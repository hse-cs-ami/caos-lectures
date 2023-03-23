# Язык Си
## История
![](kt-dr-pdp11.jpg)
Кен Томпсон и Деннис Ричи у PDP-11

[Википедия про историю языка Си](https://en.wikipedia.org/wiki/C_(programming_language)#History)

Мы используем стандарт С11 (прошлые стандарты: C89, C99; грядёт стандарт C23).

[Черновик стандарта](https://www.open-std.org/JTC1/SC22/WG14/www/docs/n1570.pdf) (сам стандарт
можно купить у ISO за большие деньги,
там написано то же самое.)

## Что регулирует стандарт
* Implementation-defined behavior
  * Пример: представление целых чисел
* Unspecified behavior 
  * Пример: порядок вычисления аргументов
при вызове функции
* Undefined behavior (UB)
  * Пример: выход за границы массива

[Статья про то, зачем нам нужен UB](https://www.ralfj.de/blog/2021/11/18/ub-good-idea.html)

```c
*p = 1;
if (!p) {
    fprintf(stderr, “NULL pointer\n”);
    return;
}
```

## Целые типы
```c
char        signed char         unsigned char
short     = signed short        unsigned short
int       = signed int          unsigned int
long      = signed char         unsigned char
long long = signed long long    unsigned long long
```

* CHAR_BIT >= 8
* sizeof(char)== 1
* sizeof(signed T) == sizeof(unsigned T) == sizeof(T)
* short как минимум 16 бит
* int как минимум 16 бит
* long как минимум 32 бита
* long long как минимум 64 бита
* <stdint.h>: intN_t, uintN_t
* <inttypes.h>: I/O макросы

sizeof для типов:
```c
Тип       Atmel AVR  32-bit  Win64  64-bit
char      1          1       1      1
short     2          2       2      2
int       2          4       4      4
long      4          4       4      8
long long -          8       8      8
__int128  -          -       -      16
```

```c
#include <limits.h>
CHAR_MIN, CHAR_MAX, SCHAR_MIN, SCHAR_MAX,
UCHAR_MIN, ..., INT_MIN, INT_MAX, UINT_MAX,
LONG_MIN, LONG_MAX, ULONG_MAX, LLONG_MIN,
LLONG_MAX, ULLONG_MAX
```

Заголовочный файл `<stdint.h>`:
* Знаковые типы: `int8_t, int16_t, int32_t, int64_t`
* Беззнаковые типы: `uint8_t, uint16_t, uint32_t, uint64_t`
* Типы размера, достаточного для хранения
  адреса в памяти: `intptr_t, uintptr_t`
* `int8_min_t` (минимум 8 бит, но может быть больше, если компьютером не поддерживается вариант с 8 битами), `int8_fast_t`(миниум 8 бит, но больше, если вычисления так производить быстрее)

Литералы:
```c
0   // int
0U  // unsigned
0UL // unsigned long
0LL // long long
```

## Знаковая и беззнаковая арифметика
Unsigned – по модулю \\(2^N\\). Обнаружить переполнение никак нельзя.

Signed – операции с неправильным ответом, как правило, UB.

```c
    UINT_MAX + 1;  // OK, always 0
    INT_MAX + 1;   // UB
    int i;
    i < i + 1;     // always true
```

Опции gcc:
* `-ftrapv` – abort() при знаковом целочисленном
переполнении
* `-fwrapv` – выполнять знаковые операции по
модулю 2^N, но компилятор не делает
никаких оптимизационных предположений
* `-fsanitize=undefined` – частичная проверка на undefined
behavior при работе программы

Для обнаружения переполнения в знаковой арифметике можем воспользоваться [GCC built-in overflow detection](https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html)

**Integer promotion.**

Числа короче, чем int (то есть char,short) при смешивании их в операциях превратятся в int, если он сможет их представить, либо в тип unsigned int

Обычные арифметические преобразования 

знаковое → беззнаковое, если оперируем знаковым и безнаковым, если беззнаковые шире или такое же по размеру

### Hello World

```c
#include <stdio.h>

#define GREETING "Hello, world!\n"
// макрос будет раскрыт в ходе препроцессирования

int main(void) {
    printf("Hello, world!\n");
    return 0;
}
```

```c
int main(){} //объявление функции с неизвестными параметрами
```

```c
// запуск
gcc hello.c -o hello
./hello
gcc -c hello.c - делает объектный файл
gcc -S hello.c - делает ассемблерный файл
gcc -E hello.c - остановка на стадии препроцессирования
gcc -Wall -Werror // show all warnings, warnings will be treated as errors
```

Исполнение начинается с нахождения функции main: если она не нашлась - ошибка компоновки. Main - единственная функция, которая может ничего не возвращать, и это не приведет к UB. В таком случае будет считаться, что main возвращает 0

```c
#include <stdio.h>

#define GREETING "Hello, world\n"

int foobar() {}

int main(void) {
    printf(GREETING);
    int x = foobar(); // UB 
    return 0;
}
```

### Классы хранения переменных

```c
#include <stdio.h>

#define GREETING "Hello, world\n"

int x; // статический класс хранения

int main(void) {
    int y; // автоматический класс хранения
    printf(GREETING);
    return 0;
}
```

- **Автоматический** : переменная объявлена внутри функции (создается при своем объявлении, перестает существовать при завершении функции, то есть живет на стеке). Если не инициализировать, то остается не инициализированной - при использовании будет ub.
- **Статический:** существует все время, пока существует программа (попадает в секцию data/bss). Автоматически инициализируются 0 (по стандарту), если их не инициализировать.
- **Регистровый**  - нет адреса в памяти

```c
register int r;
```

### Extern, static

**Объявить переменную, определенную в другой единице трансляции**

```c
extern int x;
```

**Ключевое слово static**

Для статической переменной сообщает компилятору, что в других единицах трансляциях эту переменную нельзя увидеть (то есть переменная перестанет быть экспортируемой (так по умолчанию))

Для автоматической переменной ключевое слово static показывает, что она находится в статической памяти [у нее статический класс хранения]. У нее будет скрытый класс линковки (нельзя увидеть в другой единице трансляции)

```c
static int x;
```

### Запись заголовочных файлов: пользовательских и системных

```c
//incr.h
int incr(void);
```

```c
#include <stdio.h> // системный
#include "incr.h"  // пользовательский

#define GREETING "Hello, world\n"

int main(void) {
    printf(printf("x is %d\n", incr()));
    return 0;
}
```

### Declarations and definitions

Объявления функций сколько угодно раз, определние только 1.

Заголовочный файл может быть включен в код несколько раз, но может все сломать. Чтобы избежать, используем макрос:

```c
//incr.h
#ifndef INCR_H
#define INCR_H

static int foo() {
    return 7;
}

int incr(void);

#endif
```

Почти для всех компиляторов вместо этого можно писать  `#pragma once`

### Структуры

Без ключевого слова struct pair не является именем типа. Автоматической инициализации нет, но есть агрегатная. 

```c
#include <stdio.h>
#include "incr.h"

struct pair { //
    char b;
    int i;
    long long l;
};

int main(void) {
    struct pair p1 = {1, 2, 3}; // инициализация 1
    struct pair p = {.b = 1, .l = 2}; // инициализация 2
// В 2 варианте кого не инициализировали - тот 0
    printf("x is %d\n", p.i);
}
```

### Выравнивание типов

В си происходит автоматически. Обычно выравнивание равно размеру типа, но не больше размера машинного слова(int) на данной платформе. Например, на 32битной платформе выравнивание `long long` будет 32 бита.

Чтобы элементы структуры выровнялись, компилятор вставляет между ними *padding* - пустые байты. У структуры в целом выравнивание максимальное из всех ее членов.

```c
#include <stdio.h>
#include "incr.h"

struct pair { // 32 bit platform
    char b;   // 3 empty bytes
    int i;    // 0 empty bytes
    long long l;
};
```

**Как попросить компилятор не добавлять паддинг**

```c
#include <stdio.h>
#include "incr.h"

struct pair { // 32 bit platform
    char b;   // 3 empty bytes
    int i;    // 0 empty bytes
    long long l;
} __atribute__((packed));
```

### Массивы

```c
// пустой инициализатор невалиден
int arr[10] = {0} // остальные инициализируются автоматически
// можно инициализировать произвольный элемент массива
int arr[10] = {[5] = 4};

// кол-во элементов = кол-во инициалированных
int arr[] = {1,2,3}
```

Выход за границы массива - ub. Узнать размеры массива можно через макрос.

```c
#define ARRLEN(a) (sizeof(a) / sizeof((a)[0]))

int main() {
    int arr[10] = {[5] = 4};
    for (int i = 0; i < ARRLEN(arr); ++i) {
        printf("%d\n", arr[i]);
    }
}
```

### Указатели

```c
&p; // взять адрес переменной
int* p = &arr[0]; // указатель
```

1. Разыменование указателя
    
    ```c
    *p;
    ```
    
2. Арифметика 
    
    Прибавить целое число - отступить столько эл-ов в массиве (к адресу указателя прибавляется это число * sizeof(type))
    
    ```c
    int *ptr = &arr[0];
        for (int i = 0; i < ARRLEN(arr); ++i) {
            printf("%d\n", *(ptr + i));
        }
    ```
    
    равнозначно
    
    ```c
    int *ptr = &arr[0];
        for (int i = 0; i < ARRLEN(arr); ++i) {
            printf("%d\n", ptr[i]);
        }
    ```
    
    Для `int arr[n]`, `int* ptr = arr[0]`:
    `ptr+n` - еще валидно, но разыменовать нельзя
    
    `ptr-1` уже невалидно, как и `ptr+11` - будет ub
    

### Строки

- массив байт, заканчивающийся на `\0`

```c
char[] c = "hello world\n"; // лежит на стеке
c[0]="H"; //можно менять, но длину строки менять нельзя

//такую строку не получится поменять, она лежит в rowdata
char* str = "hello world\n";
```

Длина строки `char* str = "hello world\n"`:

```c
int strlen(char* s){
    int i;
    for (i = 0; s[i] != '\0'; ++i);
    return i;
}

int main(void){
    char* str =  "hello world\n";
    printf("%d %s", strlen(str), str);
}
```

### [Printf](https://man7.org/linux/man-pages/man3/printf.3.html)

```c
int printf(const char *restrict format, ...);
```

Как работает printf: 

1. Принимает строчку с форматом
2. Читает строку, печатает обычные символы, пока не дойдет до символа %
3. Смотрит спецификатор формата

```c
"%d" - int
"%s" - char*
"%x" - int в 16ричной системе счисления, "%o" - в 8ричной
```

Спецификатор размера

```c
h - short
l - long int
```

```c
short s = 7;
printf("%d %hd", strlen(str), s); // integer promotion of s
```

### [Scanf](https://man7.org/linux/man-pages/man3/scanf.3.html)

```c
int scanf(const char *restrict format, ...);
```

- Тут очень важен спецификатор размера числа
- Возвращает кол-во считанных элементов; если scanf ничего не считал, возвращает eof

**Ввод числа**

```c
int main(void) {
    short shint = 7;
    int result = scanf("%hd", &shint);
    if (result == EOF) {
        printf("empty input\n");
        return 1;
    }
    if (result != 1) {
        printf("error\n");
        return 1;
    }
}
```

**Ввод строки** - читает до 1 пробела

```c
int main(void) {
    char buf[100];

    int result = scanf("%s", buf);
    if (result != 1) {
        printf("error\n");
        return 1;
    }
    printf("buf : %s\n", buf);
}
```

Чтобы не начать запись за границами буфера:

```c
int main(void) {
    char buf[3] = {0};

    int result = scanf("%2s", buf);
    if (result != 1) {
        printf("error\n");
        return 1;
    }
    printf("buf : %s\n", buf);
}
```

### Символы вне кодровки ASCII - Unicode

В ASCII 127 символов, помещаются в 1 байт. Unicode - кодировка, где каждый символ имеет номер code point.

```python
// python
ord('f')  // 102
chr(1100) // ь
```

В качестве кодировки code points используется теперь в основном utf-8. Она запихивает все символы в байтовую строку. Символы ascii записываются теми же кодами. Вне ascii  - распилим число на отдельные участки бит, старшими битами будем кодировать их  последовательность                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           

```c
0xxxxxxx //символ из ascii
110xxxxx 10xxxxxxx //  сколько единичек в 1 слове, столько слов и будет 
```

```c
chr(1100).encode('utf-8')
// **110**10001 **10**001100 
```

## Структура программы

Declarations and definitions

Translation unit

Extern, static

## Агрегатные типы

Arrays and structures

Aggregate initializers

Array dimensions

Pointer arithmetic

Typedef

Alignment

## Другие темы

Hosted vs freestanding

Stdio

Argc/argv

Strings

Restrict

Noreturn etc

VLA

Function pointers
