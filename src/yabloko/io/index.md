## Операционная система

Комплекс программ.

Организует доступ к ресурсам ЭВМ:
* унифицирует,
* разграничивает,
* мультиплексирует,
* учитывает и квотирует.

Для всего этого требуется привилегированный режим
работы процессора.
Часть ОС, использующую этот режим, принято называть *ядром*
(kernel).

Ядро бывает:
* монолитное,
* модульное (например, Linux),
* микроядро (microkernel).

## Взаимодействие с устройствами

### Port-mapped IO

Отдельный механизм для обмена данными с устройствами,
инициируемого специальными инструкциями процессора.

Работа с портами: инструкции [in](https://www.felixcloutier.com/x86/in), [out](https://www.felixcloutier.com/x86/out).

Например, VGA использует, кроме прочих, порты 0x3d4 и 0x3d5.

### Memory-mapped IO
Memory-mapped и port-mapped IO.

![](system_bus.png)

Например, видеопамять 


Polling.

## Загрузка компьютера с BIOS
(сейчас на вашем x86-совместимом компьютере UEFI, но идея та же)

Часть адресов RAM отображена на ROM, где лежит firmware.

[Структура адресуемой памяти x86](https://wiki.osdev.org/Memory_Map_(x86))

При включении компьютера процессор работает в режиме совместимости
с IBM PC (16-битный *real mode*), исполнение начинается
по адресу 0xFFFF0 (*reset vector*).

### Загрузка с диска
BIOS инициализирует и тестирует оборудование компьютера,
а затем читает с загрузочного диска первый *сектор* (512 байт)
и передаёт ему управление.

## Учебная ОС yabloko
[Гитхаб](https://github.com/hse-cs-ami/yabloko-public).

Учебную операционную систему мы запускаем в эмуляторе. Скрипт установки под macOS и Linux находится в файле `setup.sh`.

Среди файлов есть `mbr.S` (master boot record). Он должен умещаться в 512 байт и будет складываться в первый сектор эмулируемого жесткого диска.

### DRIVERS

В  `drivers/port.h` лежат обертки port-mapped IO для языка C, написанные в виде ассемблерных вставок:

```c
static inline unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
		// инструкция in, берет номер порта из регистра %dx, а кладёт результат в регистр %al
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}
```

Нам нужно что-то печатать на экран, для этого есть `drivers/vga.h` и `drivers/vga.с` для работы со стандартным графическим адаптером VGA. Когда компьютер загружается, экран работает в текстовом режиме и содержит 25 строк и 80 столбцов. Линейно лежат пары байт: в одном — символ, в другом — цвет фона и цвет самого символа. Символы нумеруются построчно начиная с левого верхнего края экрана.

```c
char* const video_memory = (char*) 0xb8000; // видео-память отображена по адресу 0xb8000 (memory-mapped IO)

void vga_set_char(unsigned offset, char c) {
    video_memory[2 * offset] = c;                                  // сам символ
    video_memory[2 * offset + 1] = get_color(light_gray, black);   // цвет; пишем светло-серым по черному
}
```

Так как мы не всегда будем пользоваться эмулятором в графическом режиме (не всегда будет настоящий монитор), необходимо эмулировать также последовательный порт, способный передавать байты. Код в `drivers/uart.h` и `drivers/uart.c`

### KERNEL

Посмотрим на `kernel.c`:

```c
void _start() {
    uartinit();                  // запускаем последовательный порт

    vga_clear_screen();          // очищаем экран
    printk("\nYABLOKO\n");       // выводим текст
    asm("hlt");                  // остановиться и ничего не делать
}
```

Заметим несколько вещей в `kernel.c`:

1. Функция `vga_clear_screen()` заполняет весь экран пробелами.
2. `printk("\nYABLOKO\n")` выводит текст не только на экран, но и на последовательный порт. На данный момент у нас не реализованы прокрутка экрана и курсор; мы не умеем работать с клавиатурой и вводом.
3. У нас нет способа выключить компьютер; нам придется делать это платформо-специфичным путём с помощью `qemu_shutdown()` 

Существует две реализации языка C:

- Hosted реализация подразумевает, что есть операционная система, есть стандартная библиотека языка C и реализованные в ней функции. На эту реализацию мы рассчитываем при запуске компилятора gcc.
- Freestanding реализация используется для ядра, подразумевает, что мы работаем без операционной системы, сами организуем загрузку бинарника и начало исполнения в нужной точке. Особенность freestanding режима — отсутствие функции main(); пишем _start — это стандартное название для точки входа в ELF-бинарник. При сборке передаем опцию `-ffreestanding`, чтобы работать именно с этой реализацией.

Так же важный момент сборки `-Ttext 0x1000` — мы указываем, с какого адреса начинается секция `text`. В адресуемой памяти x86 этот адрес как раз попадает в диапазон usable memory. 

### MASTER BOOT RECORD

BIOS читает загрузчик в память по адресу 0x7c00 и на этот же адрес передает выполнение. Задача — скомпоновать mbr так, чтобы он с этого адреса начинался.

Чтобы запустить под отладчиком целый компьютер, нужен следующий кусок в Makefile

```makefile
debug-boot-nox: image.bin mbr.elf
	qemu-system-i386 -nographic -drive format=raw,file=$< **-s -S** &  // "взаимодействуй с отладчиком и подожди, пока отладчик подключится"
	$(GDB) mbr.elf \
		-ex "set architecture i8086" \                               // архитектура 8086 (16-битная архитектура)
		-ex "target remote localhost:1234" \                         // эмулятор по адресу localhost:1234 запустит gdb сервер и будет ждать, когда подключится отладчик 
		-ex "break *0x7c00" \                                        // breakpoint на адрес входа в загрузчик
		-ex "continue"                                               // продолжить исполнение
```

Посмотрим ближе на mbr.S под отладчиком:

```x86asm
.code16
    .global _start
_start:
// по договоренностям в %dl BIOS передает загрузчику номер диска,
// с которого он загрузчик прочитал; поэтому этот регистр сохраняется в boot_drive
    mov %dl, boot_drive                     
    mov $banner, %si
    call print_string                         // посимвольная печать banner

    call get_drive_geometry
    call load_kernel
    call switch_to_32bit

    hlt
    jmp . // loop forever
```

Для ввода-вывода в загрузчике используются функции BIOS. Пример можно увидеть в `print_string`: в регистр `%ah` кладётся магическое значение 0x0e, а в `%al` кладется символ; при вызове  `int $0x10` символ печатается на экран. Следующий код печатает посимвольно строку:

```x86asm
.code16
print_string:
    mov $0x0e, %ah  // "teletype output"
repeat:
    lodsb           // equivalent to mov (%si), %al; inc %si

    test %al, %al
    je done

    int $0x10       // bios interrupt
    jmp repeat
done:
    ret
```

В `fs/fs.h` описана структура файловой системы: первый сектор она использует для собственных нужд, со следующего сектора начинается первый файл. Пока что у нас один файл kernel.bin, он находится начиная с третьего сектора диска.

```
/* Directory structure:
         32-byte entries
┌───────────────────────────────┐
│Reserved                       │
│char[32]                       │
├──────┬──────┬────────┬────────┤
│Offset│Size  │Reserved│Name    │
│uint32│uint32│uint32  │char[20]│
├──────┼──────┼────────┼────────┤
│ ...  │      │        │        │
Offset is in sectors (zero-based),
size is in bytes, name is 0-terminated.
*/
```

Результирующий образ диска состоит из загрузочного сектора и файловой системы.

Так как нет встроенного драйвера жесткого диска, мы читаем с диска секторы с ядром с помощью BIOS, как и загрузчик. Реализация находится в функции `load_kernel` в `mbr.S`. Результат работы этой функции:

- В памяти оказывается ядро (то, что идёт после заголовка в ELF-файле)
- В переменной entry оказывается адрес точки входа, на которую мы позже будем передавать управление.

Чтобы загрузить ELF-файл нам надо прочитать программные заголовки и для каждого из них загрузить секцию по соответствующему адресу. Но у нас это реализовано по-другому: мы рассчитываем, что ELF-файл можно загрузить в память одним куском без разграничения доступа, защиты памяти. Мы также находимся в 16-битном режиме и используем функции BIOS (см. `int 0x13`)

После `load_kernel` мы переходим в 32-битный режим, так как ядро мы собирали как
32-битный бинарник. Поскольку IBM PC умел адресовать только 1МБ памяти, у него
было 20 дорожек на адресной шине (\\(2^{20}\\) байт = 1 Мбайт): A0-A19. С появлением
компьютеров, способных адресовать большее количество памяти, возникла проблема
совместимости:  старый софт рассчитывал, что в адресе больше 1МБ старшие биты
адреса обрезаются. Для того чтобы вывести компьютер из этого режима
совместимости, нужно провести некоторые манипуляции (первые две строки функции):

```x86asm
switch_to_32bit:
    mov	$2, %al
    out	%al, $0x92          // enable A20

    cli                     // 1. disable interrupts

// в процессоре есть специальный регистр, указывающий на gdt, и мы загружаем таблицу в него:
    lgdt gdt_descriptor     // 2. load GDT descriptor

// меняем младший бит в регистре %cr0; так мы переходим из real mode в protected
    mov %cr0, %eax
    or $1, %eax             // 3. enable protected mode
    mov %eax, %cr0

// мы не можем загрузить %cs напрямую с помощью mov, поэтому пользуемся long jump - он загружает %cs и %eip
    ljmp $SEG_KCODE << 3, $init_32bit // 4. far jump
```

Сегментные регистры cs, ds, cs, ds, ss, es, fs, gs теперь означают не смещение в
памяти, а индекс в таблице дескрипторов (Global Descriptor Table) — структуре
данных, где указаны сегменты и их особенности (например, код ядра должен
исполняться только на высоком уровне привилегий; старый 16-битный код должен
исполняться в режиме совместимости). Таблицу дескрипторов можно увидеть ниже:

```x86asm
gdt_start:
    .quad 0x0                                // null descriptor, необходимость
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)    // code seg, доступный на чтение и исполнение, адрес от 0x0 до 0xffffffff
    SEG_ASM(STA_W, 0x0, 0xffffffff)          // data seg, доступный на запись (и автоматически на чтение)
gdt_end:
```

То, как раскрываются макросы, можно посмотреть в `cpu/gdt.h`

Переключившись в 32-битный режим, мы должны создать стек и переключиться на исполнение кода из `kernel.c`:

```x86asm
mov $KERN_STACK_BASE, %ebp   // создание стека
    mov %ebp, %esp

    movzwl entry, %esi       // сохраненный адрес точки входа
    call *%esi               // переключение на исполнение кода ядра
```
