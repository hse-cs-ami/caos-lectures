# Summary

[Как устроен курс](./01-course.md)
- [Компьютеры](./02-computers.md)
- [Целые числа](./03-integers.md)
- [Язык ассемблера](./04-assembly.md)
  - [Переходы](./05-jumps.md)
  - [Обращение к памяти](./07-memory.md)
  - [Подпрограммы](08-call-ret/08-call-ret.md)
  - [Компоновка](09-elf/index.md)
- [Устройство компьютера]()
  - [Как сделать CPU из транзисторов](./06-nand2cpu.md)
  - [Память и кэш](hardware/ram-and-cache.md)
  - [Внутренности современных процессоров](x1-cpu-internals/index.md)
- [Язык Си](10-c/index.md)
  - [Динамическая память](11-c-continued/index.md)
  - [Bits and pieces](12-c-bit-twiddling/index.md)
  - [Стандартная библиотека](c-stdlib/index.md)
- [Вещественные числа](18-floating-point-arithmetic/index.md)
  - [Поддержка на x86: FPU, MMX, SSE, AVX](18-x86-floating-point/index.md)
- [ИПР](ipr/index.md)
  - [Git](ipr/git/git.md)
  - [Шелл](ipr/shell/shell.md)
  - [Системы сборки](ipr/build/build.md)
  - [Контейнеры](ipr/containers/containers.md)
- [«Системное программирование»](yabloko/index.md)
  - [Взаимодействие с устройствами](yabloko/io/index.md)
  - [Прерывания](yabloko/interrupts/Interrupts.md)
  - [Кольца защиты](yabloko/rings/rings.md)
- [POSIX](posix/index.md)
  - [Файлы](posix/files/index.md)
  - [Процессы](posix/processes/index.md)
  - [Страничная виртуальная память](posix/mmap/mmap.md)
  - [Пользователи и группы](posix/users/users.md)
  - [Сигналы](posix/signals/index.md)
  - [File descriptions](posix/file-descriptions/file-descriptions.md)
  - [Каналы (pipes)](posix/pipes/pipes.md)
  - [Файловая система](posix/file-system/index.md)
- [Динамическая компоновка](dynamic-linking/index.md)
- [Сети и сокеты](22-sockets/index.md)
  - [Мультиплексирование](23-mplex/mplex.md)
- [Работа с общей памятью](25-shared-mem/index.md)
    - [RMW](26-parallel-2/index.md)
- [Использование GPU](gpu/gpu.md)
- [Темы экзамена по ОС](exam/exam.md)