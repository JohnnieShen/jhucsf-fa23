/* countToN.S */

	.section .rodata
sPrompt:    .string "Enter an integer: "
sInputFmt:  .string "%ld"
sOutputFmt: .string "%ld\n"

	.section .text

	.globl main
main:
	pushq %r12              /* save original value of %r12 */
	pushq %r13              /* save original value of %r13 */
	pushq %rbp              /* save original value of %rbp */
	subq $16, %rsp          /* allocate space for local var(s) */
	movq %rsp, %rbp         /* make %rbp point to local var area */

	movq $sPrompt, %rdi     /* pass prompt string to printf */
	call printf             /* call printf */

	movq $sInputFmt, %rdi   /* pass format string to scanf */
	movq %rbp, %rsi         /* pass addr of local var (for input val) */
	call scanf              /* read input value */

	movq (%rbp), %r13       /* value to count to (N) */
	movq $1, %r12           /* initialize counter */
.Ltop:
	movq $sOutputFmt, %rdi  /* format string arg to printf */
	movq %r12, %rsi         /* value to print */
	call printf             /* call printf */
	incq %r12               /* increment counter */
	cmpq %r13, %r12         /* check whether counter <= N */
	jle .Ltop               /* if so, continue loop */

	addq $16, %rsp          /* deallocate local vars */
	popq %rbp               /* restore original value of %rbp */
	popq %r13               /* restore original value of %r13 */
	popq %r12               /* restore original value of %r12 */
	movl $0, %eax
	ret
