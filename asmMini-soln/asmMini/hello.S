/* hello.S */

	.section .rodata
sHelloMsg: .string "Hello, world\n"

	.section .text

	.globl main
main:
	subq $8, %rsp
	movq $sHelloMsg, %rdi
	call printf
	movl $0, %eax
	addq $8, %rsp
	ret
