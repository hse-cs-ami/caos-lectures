## Семейство юниксовых
Картинка из Википедии:
![](https://upload.wikimedia.org/wikipedia/commons/c/cd/Unix_timeline.en.svg)

POSIX: 1988

Single UNIX Specification (SUS): 1994

## Файлы
Файл — последовательность байт, к которой ОС предоставляет
доступ посредством *файловых операций*.

Например:
* файл на диске;
* вывод программы;
* пользовательский ввод;
* последовательность случайных чисел;
* всё содержимое диска;
* ...


```
    ───────────────────────────
... Mary had a little lamb, its ...
    ───────────────────────────
          ▲
          │ file position

read(1) → "a"
read(5) → "d a l"
read(5) → "ittle"
write(5, " wolf")

    ───────────────────────────
... Mary had a little wolf, its ...
    ───────────────────────────
                          ▲
                          │
```

Конец файла (EOF):

```
    ─────────────────────────┐
... il sole e l'altre stelle.│
    ─────────────────────────┘
                      ▲
                      │

read(10) → "stelle."
read(10) → ""
write(5, " FIN.")

    ──────────────────────────────┐
... il sole e l'altre stelle. FIN.│
    ──────────────────────────────┘
                                  ▲
                                  │
```

## Файловые дескрипторы

```
               char buf[10];
               int fd = 3;
               read(fd, buf, sizeof(buf));
Userspace
──────────────────────────────────────────
Kernelspace
               0 → user input
               1 → terminal
               2 → terminal
               3 → somefile.txt
```


* 0 (`STDIN_FILENO`) — standard input
* 1 (`STDOUT_FILENO`) — standard output
* 2 (`STDERR_FILENO`) — standard error output 
