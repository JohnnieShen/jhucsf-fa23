/*
 * Assembly language main function implementation
 */

#define MAX_WORDLEN 63

	.section .text

    .section .bss


	.globl main
main:
    subq $8, %rsp

    pushq %r12  //use r12 to store the total number of words
    pushq %r13  //use r13 to store the number of unique words
    pushq %rbp  //use rbp to store the longest string
    pushq %rbx  //use rbx to store the best word count
    pushq %r15







    popq %r15
    popq %rbx
    popq %rbp
    popq %r13
    popq %r12

    addq $8, %rsp
	ret

/*
vim:ft=gas:
*/
