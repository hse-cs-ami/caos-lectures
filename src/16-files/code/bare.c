#include <sys/syscall.h>

void _start() {
    asm volatile ("int $0x80" : : "a"(SYS_exit), "b"(42));
}
