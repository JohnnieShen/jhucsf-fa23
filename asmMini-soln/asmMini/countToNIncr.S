/* countToNIncr.S */

	.section .rodata
sPromptMax: .string "Enter maximum: "
sPromptIncr:.string "Enter an integer: "
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

	/* Local variables:
	 *
	 * %rbp+8 -->   [maximum to count to]
	 * %rbp+0 -->   [increment]
	 */

	movq $sPromptMax, %rdi  /* pass prompt string to printf */
	call printf             /* call printf */

	movq $sInputFmt, %rdi   /* pass format string to scanf */
	leaq 8(%rbp), %rsi      /* pass addr of max local var */
	call scanf              /* read input value */

	movq $sPromptIncr, %rdi /* pass prompt string to printf */
	call printf             /* call printf */

	movq $sInputFmt, %rdi   /* pass format string to scanf */
	leaq 0(%rbp), %rsi      /* pass addr of incr local var */
	call scanf              /* read input value */

	movq 8(%rbp), %r13      /* value to count to (N) */
	movq $1, %r12           /* initialize counter */
.Ltop:
	movq $sOutputFmt, %rdi  /* format string arg to printf */
	movq %r12, %rsi         /* value to print */
	call printf             /* call printf */
	addq 0(%rbp), %r12      /* increase counter by increment value */
	cmpq %r13, %r12         /* check whether counter <= N */
	jle .Ltop               /* if so, continue loop */

	addq $16, %rsp          /* deallocate local vars */
	popq %rbp               /* restore original value of %rbp */
	popq %r13               /* restore original value of %r13 */
	popq %r12               /* restore original value of %r12 */

	movl $0, %eax
	ret
