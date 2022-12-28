## Bits and pieces

Иногда хочется работать с отдельными битами внутри значения.

Вспомним кодировку UTF-8:
```c
0xxxxxxx — cимвол ASCII
110xxxxx 10xxxxxx
1110xxxx 10xxxxxx 10xxxxxx
11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
```

Операторы сдвига я языке Си:
 ```c
 x << n
 x >> n
 ```

Undefined behaviour, если:
> — An expression is shifted by a negative number or by an amount greater than or equal to the width of the promoted expression (6.5.7).
> — An expression having signed promoted type is left-shifted and either the value of the expression is negative or the result of shifting would be not be representable in the promoted type (6.5.7).

Например:
```c
7 << 2           // 28
7 << -1          // UB
7 << 30          // на нашей платформе UB
-1 >> 1          // implementation defined
0xABCDEFFFu << 4 // 0xBCDEFFF0u
0 << 32          // на нашей платформе UB
```

Давайте пробовать классифицировать байты из UTF-8:
```c
_Bool is_ascii(char b) {
    // return (b >> 7) == 0; // nope, could be signed char
    return ((unsigned char) b >> 7) == 0;
}

_Bool is_continuation(unsigned char b) {
    // return (b >> 6) == 0b10; // valid in C++14
    return (b >> 6) == 2;
}

_Bool is_2_byte_start(unsigned char b) {
    return (b >> 5) == 6; // 0b110
}
```

Вместо сдвигов можно воспользоваться битовыми масками:
```c
_Bool is_2_byte_start(unsigned char b) {
    return (b & 0xE0) == 0xC0; // 0b1110'0000, 0b1100'0000
}
```

Битовые поля:
```c
struct u8char {
    uint8_t sign_bit : 1;
    uint8_t tail_bits : 7;
}

union char_breaker {
    uint8_t number;
    struct u8char fields;
}

...
union char_breaker cb = {.number = 'x'};
cb.fields.sign_bit; // implementation defined if this is the sign bit :-(
```

## `setjmp` / `longjmp`

```c
#include <setjmp.h>
int setjmp(jmp_buf env);
void longjmp(jmp_buf env);
```

## Inline assembly
[Документация GCC](https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html)
```c
// Basic asm
asm("nop");

// Extended asm
// asm(template : outputs : inputs : clobbers)
```
