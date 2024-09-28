    .intel_syntax noprefix
    .global main
    jmp label  // relative jump
main:
    lea rax, [rip + label]
    jmp rax    // absolute jump
    jmp label  // relative jump
    nop
label:
    xor eax, eax
    ret 