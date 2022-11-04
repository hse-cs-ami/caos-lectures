# files

Перечитаем [open(2)](https://man7.org/linux/man-pages/man2/open.2.html):

```
       A call to open() creates a new open **file description**, an entry in
       the system-wide table of open files.  The open file description
       records the file offset and the file status flags.  A
       **file descriptor** is a reference to an open file description; this
       reference is unaffected if *pathname* is subsequently removed or
       modified to refer to a different file.
...
       When a file descriptor is duplicated (using dup(2) or similar),
       the duplicate refers to the same open file description as the
       original file descriptor, and the two file descriptors
       consequently share the file offset and file status flags.  Such
       sharing can also occur between processes: a child process created
       via fork(2) inherits duplicates of its parent's file descriptors,
       and those duplicates refer to the same open file descriptions.
```

То есть файловый дескриптор (число) — это индекс в таблицу в ядре, из которой ссылки ведут на file descriptions.

Разыщем соответствующе структуры данных в ядре Linux. Вот так выглядит [таблица файловых дескрипторов](https://github.com/torvalds/linux/blob/5bfc75d92efd494db37f5c4c173d3639d4772966/include/linux/fdtable.h#L27):

```c
struct fdtable {
	...
	struct file __rcu **fd;      /* current fd array */
	unsigned long *close_on_exec;
	unsigned long *open_fds;
	unsigned long *full_fds_bits;
	...
};
```

А вот и [file description](https://github.com/torvalds/linux/blob/5bfc75d92efd494db37f5c4c173d3639d4772966/include/linux/fs.h#L917):

```c
struct file {
	...
	struct path		f_path;
	struct inode		*f_inode;	/* cached value */
	const struct file_operations	*f_op;

  ...
	atomic_long_t		f_count;
	unsigned int 		f_flags;
	fmode_t			f_mode;
	struct mutex		f_pos_lock;
	loff_t			f_pos;
  ...
```

Ну и [таблица виртуальных методов](https://github.com/torvalds/linux/blob/5bfc75d92efd494db37f5c4c173d3639d4772966/include/linux/fs.h#L1888):

```c
struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	...
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	...
	int (*mmap) (struct file *, struct vm_area_struct *);
	unsigned long mmap_supported_flags;
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
  ...
```

Что происходит с этими структурами при:

- fork();
- exec();
- close();
- lseek();
- read()?

Подробности в документации по ядру:

[Overview of the Linux Virtual File System - The Linux Kernel documentation](https://www.kernel.org/doc/html/latest/filesystems/vfs.html)

Давайте перечислим известные нам типы file descriptions:

- регулярный файл (живёт на каком-то устройстве, хранит байты, которые в него записали; возможно, имеет имя в файловой системе);
- псевдофайл (это не термин, если что; например, `/proc/self/maps`);
- файл устройства;
- канал (`pipe`) — может появиться в результате pipe() или open(fifo);
- `epollfd`, `signalfd`, `eventfd`, `userfaultfd`, тысячи их.

Но это всё было только про открытые файлы...

### А что с именами файлов?

Файловые системы, которые первыми приходят в голову: `ext4`, `FAT`, `NTFS` — разные способы записать иерархическую структуру директорий и файлов на носитель данных. Также знакомая нам `procfs` — способ показать структуры данных ядра в виде псевдофайлов.

Из отдельных файловых систем мы собираем дерево с корнем в `/`: каждую файловую систему мы *монтируем* в какую-то существующую точку этого дерева, и она подменяет собой прошлое содержимое этого поддерева. Основные объекты на этом уровне — `dentry` (directory entry, имя и атрибуты файла в директории) и `inode` (сам файл или директория).

При вызове `open()` появляется `struct file`, который ссылается на свои `dentry` и `inode`.

Вся подсистема ядра Linux, которая занимается монтированием, разрешением имён, операциями с открытыми файлами и т. д., называется Virtual File System.

Интересные типы `inode`:

- символические ссылки;
- именованные каналы;
- устройства.

«Жёсткая ссылка» в этих терминах — ситуация, когда несколько `dentry` ссылаются на один и тот же `inode`. И обратно, бывают `inode` без `dentry`:

```c
int fd = open("myfile.txt", O_RDONLY);
unlink("myfile.txt");
// или
int tmpfd = open("/tmp", O_TMPFILE | O_RDWR);
```

### Именованные каналы

Создаём с помощью `mkfifo("path/to/fifo", mode)`, при открытии только на запись или только на чтение `open` заблокируется:

```bash
myltsev-nix:~/tmp$ mkfifo fifo
myltsev-nix:~/tmp$ strace cat fifo
...
openat(AT_FDCWD, "fifo", O_RDONLY   # и зависли
```

Когда FIFO открыли с обоих концов, то создаётся обычный pipe, который и привязывается к соответствующим файловым дескрипторам. Пайп без символа `|`:

```bash
$ cat /etc/passwd >fifo & grep root fifo
[1] 879281
root:x:0:0:root:/root:/bin/bash
[1]+  Done                    cat /etc/passwd > fifo
```

### Терминалы

![Untitled](files%2053940f3125bd4ef48a30d8096b728dac/Untitled.png)

(Кен Томпсон и Деннис Ритчи за телетайпом [Teletype 33](https://en.wikipedia.org/wiki/Teletype_Model_33), подключённым к компьютеру PDP-11. `The Model 33 was one of the first products to employ the newly-standardized ASCII
code, which was first published in 1963!`)

У большинства программ, которые мы запускаем, стандартные файлы 0–2 привязаны к специальному устройству — псевдотерминалу:

```bash
$ ls -l /proc/self/fd
total 0
lrwx------ 1 myltsev myltsev 64 Apr 14 16:39 0 -> /dev/**pts**/0
lrwx------ 1 myltsev myltsev 64 Apr 14 16:39 1 -> /dev/**pts**/0
lrwx------ 1 myltsev myltsev 64 Apr 14 16:39 2 -> /dev/**pts**/0
lr-x------ 1 myltsev myltsev 64 Apr 14 16:39 3 -> /proc/879372/fd
```