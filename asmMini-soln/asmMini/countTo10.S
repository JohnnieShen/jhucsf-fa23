/* countTo10.S */

	.section .rodata
sOutputFmt: .string "%ld\n"

	.section .text

	.globl main
main:
	pushq %r12              /* save original value of %r12 */

	movq $1, %r12           /* initialize counter */
.Ltop:
	movq $sOutputFmt, %rdi  /* format string arg to printf */
	movq %r12, %rsi         /* value to print */
	call printf             /* call printf */
	incq %r12               /* increment counter */
	cmpq $10, %r12          /* check whether counter <= 10 */
	jle .Ltop               /* if so, continue loop */

	popq %r12               /* restore original value of %r12 */
	movl $0, %eax
	ret
