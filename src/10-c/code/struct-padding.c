#include <stdio.h>

struct example { // aligned to 8 bytes
    char b;
    // 3 padding bytes
    int i;
    // no padding bytes
    long long l;
    char c;
    // 7 padding bytes
};

int main() {
    printf("sizeof(struct example) == %lu, _Alignof(struct example) == %lu\n",
            sizeof(struct example), _Alignof(struct example));
}
