#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <wctype.h>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    if (argc >= 3) {
        printf("strcoll([%s], [%s]) is %d\n", argv[1], argv[2], strcoll(argv[1], argv[2]));
    } else if (argc == 2) {
        wchar_t c = 0;
        if (mbtowc(&c, argv[1], strlen(argv[1])) > 0) {
            printf("Character code is %d\n", c);
            printf("iswupper: %d\n", iswupper(c));
        } else {
            puts("invalid multibyte sequence");
        }
    } else {
        printf("1000×Pi is %'f\n", 1000 * M_PI);
    }
}