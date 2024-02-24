## Многофайловая сборка

```
     preprocess        compile        assemble        │
.c ──────────────► .i ──────────► .s ──────────► .o   │
                                                      ├──────►  Executable
                    preprocess        assemble        │ link
               .S ──────────────► .s ──────────► .o   │
                                                      │
```

Опции gcc:
```
-E  stop after preprocessing
-S  stop after compiling to assembly
-c  stop after assembling the object file
```

## Формат ELF

[Википедия](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

Рассмотрим простую программу:
```x86asm
// print42.S
    .global main
main:
    mov wrong, %eax
    mov answer, %eax
    call writei32
    call finish

    .data
wrong:
    .int 41
answer:
    .int 42
```

```
$ gcc -m32 -c print42.S -o print42.o
$ gcc -m32 print42.o simpleio_i686.S -o print42
$ nm print42.o
00000004 d answer
         U finish
00000000 T main
         U writei32
00000000 d wrong
$ objdump -d print42
...
080497a5 <main>:
 80497a5:       a1 68 c0 10 08          mov    0x810c068,%eax
 80497aa:       a1 6c c0 10 08          mov    0x810c06c,%eax
 80497af:       e8 5c 00 00 00          call   8049810 <writei32>
 80497b4:       e8 b7 00 00 00          call   8049870 <finish>
...
$ objdump -dr print42.o
00000000 <main>:
   0:   a1 00 00 00 00          mov    0x0,%eax
                        1: R_386_32     .data
   5:   a1 04 00 00 00          mov    0x4,%eax
                        6: R_386_32     .data
   a:   e8 fc ff ff ff          call   b <main+0xb>
                        b: R_386_PC32   writei32
   f:   e8 fc ff ff ff          call   10 <main+0x10>
                        10: R_386_PC32  finish
```