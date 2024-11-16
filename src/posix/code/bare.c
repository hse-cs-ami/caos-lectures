#include <sys/syscall.h>
#include <unistd.h>

void _start() {
    asm("syscall" : : "a"(SYS_exit), "D"(42));
}