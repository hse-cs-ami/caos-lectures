#include <sys/syscall.h>

    .global _start
_start:
    mov $SYS_exit, %eax
    xor %ebx, %ebx
    int $0x80
