#include <sys/syscall.h>

    .intel_syntax noprefix
    .global _start
_start:
    mov eax, SYS_exit
    mov rdi, 42
    syscall
