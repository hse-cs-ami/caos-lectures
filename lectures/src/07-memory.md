## Обращения к памяти.
Сохранить значение регистра eax по адресу 0x40100,
а потом загрузить обратно в регистр ebx:
```x86asm
    mov %eax, 0x40100
    mov 0x40100, %ebx
```

Как правило, мы используем в качестве адресов метки:
```x86asm
    .global main
main:
    mov x, %eax
    call writei32
    call finish

x:  .int 43
```

Не любой адрес в памяти доступен для чтения и тем более записи:
```x86asm
    .global main
main:
    inc x
    call finish

x:  .int 43
```

## Секции .data и .bss
Код и данные записываются в одну из *секций* исполняемого
файла. По умолчанию это секция `.text`, в которой
ожидается машинный код и которая недоступна для записи.

Данные можно положить в секцию `.data`:
```x86asm
    inc x

    .data
x:  .int 43

    .text
    call finish
```

## Endianness

x86 — little endian:
```x86asm
    // 0x40100:  00 00 00 00  00 00 00 00
    movl $0xabcdef, 0x40100
    // 0x40100:  ef cd ab 00  00 00 00 00
```
Младший байт по младшему адресу в памяти.

## Расширение

```x86asm
    movzbl %al, %edi  // zero-extend, расширение нулями
    movsbl %al, %edi  // sign-extend, расширение знаковым битом
    cdq  // sign-extend eax to edx:eax
```

В синтаксисе Intel —
[movzx](https://www.felixcloutier.com/x86/movzx),
[movsx](https://www.felixcloutier.com/x86/movsx).

## Разные способы адресации в x86

```x86asm
    mov x + 4, %eax   // прямой
    mov $x, %esi
    mov (%esi), %eax  // косвенный
```

Общий вид обращения к памяти:

OFFSET(BREG, IREG, SCALE)

Адрес вычисляется по формуле:

BREG + OFFSET + IREG * SCALE

