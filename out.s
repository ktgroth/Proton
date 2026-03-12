global _start
section .text

helper:
	push rbp
	mov rbp, rsp

	mov rax, 100
	jmp .func_end_0
.func_end_0:
	mov rsp, rbp
	pop rbp
	ret
main:
	push rbp
	mov rbp, rsp

	sub rsp, 16

	mov rax, 10
	mov [rbp-8], rax
	call helper

	mov [rbp-16], rax
	mov rax, [rbp-8]

	push rax
	mov rax, [rbp-16]

	push rax
	mov rax, 2
	pop rdi
	imul rax, rdi

	pop rdi
	add rax, rdi

	jmp .func_end_1
.func_end_1:
	mov rsp, rbp
	pop rbp
	ret
_start:
	mov rdi, [rsp]
	lea rsi, [rsp+8]
	sub rsp, 8
	call main
	add rsp, 8
	mov rdi, rax
	mov rax, 60
	syscall
