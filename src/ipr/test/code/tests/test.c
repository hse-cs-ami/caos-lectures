#include <stdio.h>
#include "../src/dumbfunction.c"

int main() {
    if (dumbfunction() == 42) {
        puts("Test passed!");
        return 0;
    } else {
        puts("Test failed!");
        return 1;
    }
}

