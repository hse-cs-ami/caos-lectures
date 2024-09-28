    .intel_syntax noprefix
    .global main

    .data
functable:
    .quad func1
    .quad func2

    .text
main:
    jmp [rip + functable + 8]

func1:
    xor eax, eax

func2:
    xor ecx, ecx
    ret