#include <stdio.h>

#define PRINTALIGN(type) printf("_Alignof(" #type ") ==\t%lu\n", _Alignof(type))

int main(void) {
    PRINTALIGN(char);
    PRINTALIGN(short);
    PRINTALIGN(int);
    PRINTALIGN(long);
    PRINTALIGN(long long);
}
