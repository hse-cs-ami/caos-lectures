#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    void *dso = dlopen("libfun.so", RTLD_LAZY);
    if (dso == NULL) {
        fprintf(stderr, "dlopen: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    void *sym = dlsym(dso, "fun");
    if (sym == NULL) {
        fprintf(stderr, "dlsym: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    int (*fun)(int) = sym;
    printf("fun(42) == %d\n", fun(42));
}