
section .text
global _start

_start:
    push rbp
    mov rbp, rsp

    mov rax, 0x01
    mov rbx, 0x0C
    add rax, rbx

    mov [rbp - 0x08], rax

    mov rax, [rbp - 0x08]
    mov [rbp - 0x10], rax

    mov rax, 0
    mov [rbp - 0x18], rax

    mov eax, 0x04
    mov ebx, 0x01
    mov ecx, message
    mov edx, message_length
    int 0x80

    pop rbp

    mov eax, 0x01
    mov ebx, 0x00
    int 0x80

section .data
message:
    db 'Hello World', 0x0A, 0x00
message_length equ $-message
