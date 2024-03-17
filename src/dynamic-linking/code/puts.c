#include <stdio.h>


int puts(const char* s) {
    printf("Intercepted puts: %s\n", s);
}
