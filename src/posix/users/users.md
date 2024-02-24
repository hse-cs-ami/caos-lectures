## Пользователи и группы
Идентификатор пользователя (UID, user identifier) — неотрицательное целое число.

Один из способов хранить информацию о пользователях — файл `/etc/passwd`:
```shell
$ grep -A2 myltsev: /etc/passwd
myltsev:x:1000:1001:Alexander Myltsev,,,:/home/myltsev:/bin/bash
layton:x:1001:1003::/home/layton:/bin/sh
limiosil:x:1002:1004::/home/limiosil:/bin/sh
```
(Формат можно уточнить с помощью `man 5 passwd`.)

Пользователи могут состоять в _группах_, которые тоже имеют
числовые идентификаторы. Группы перечислены в `/etc/group`:
```
myltsev:x:1001:
students:x:1005:loende,alexeyha,gareton,...
```
В группе `myltsev` с идентификатором 1001 состоят только те
пользователи, у которых она указана как основная, а вот
в группе `students` есть дополнительные участники.

Пользователь с идентификатором 0 (обычно имеет имя `root`) —
суперпользователь, для которого не действуют правила
разграничения доступа.

## Атрибуты процессов
Real UID (RUID) — идентификатор пользователя, запустившего процесс.
Можно получить с помощью `getuid()`.

Effective UID (EUID) применяется для проверки прав доступа.
Можно получить с помощью `geteuid()`.

## Биты доступа к файлам
```
$ ls -ld /home/luke
drwxr-x--- 4 luke luke 4096 Sep 13 22:23 /home/luke
$ ls -l /bin/passwd
-rwsr-xr-x 1 root root 59976 Feb  6 12:54 /bin/passwd
```

Согласно `man 2 open`:
```
r--------  S_IRUSR    00400 user has read permission
-w-------  S_IWUSR    00200 user has write permission
--x------  S_IXUSR    00100 user has execute permission
---r-----  S_IRGRP    00040 group has read permission
----w----  S_IWGRP    00020 group has write permission
-----x---  S_IXGRP    00010 group has execute permission
------r--  S_IROTH    00004 others have read permission
-------w-  S_IWOTH    00002 others have write permission
--------x  S_IXOTH    00001 others have execute permission
           S_ISUID  0004000 set-user-ID bit
           S_ISGID  0002000 set-group-ID bit (see inode(7)).
           S_ISVTX  0001000 sticky bit (see inode(7)).
```

У `passwd` установлен бит SUID (set-user-ID), поэтому
при `execve` UID его владельца станет EUID процесса.