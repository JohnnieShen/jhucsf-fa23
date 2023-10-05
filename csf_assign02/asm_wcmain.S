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
    pushq %r15  //temporary space to store the string value
    pushq %r14  //use to store file input

    movq $0, %r12
    movq $0, %r13

    //rdi = number of Parameter, rsi = argv[0], rdx = argv[1]

    cmp $0, %rdi
    je .read_terminal_input

    //there is file that needs to be read on rdx
    movq %rdx, %rdi
    call fopen
    movq %rax, %r14
    jmp .read_words


.read_terminal_input:


.read_words:
    //for wc_readnext, the read value will be returned in rsi
    movq (%r14), %rdi
    movq $0, %rsi
    call wc_readnext
    cmp $0, %rax
    je .read_terminated

    movq %rsi, %r15

    //trim non alpha
    movq %r15, %rdi
    call wc_trim_non_alpha
    movq %rdi, %r15

    //to lower
    movq %r15, %rdi
    call wc_tolower
    movq %rdi, %r15

    //increase the total number of words
    incq %r12

    //use r11 as temporary pointer to store pointer











.read_terminated:



    popq %r14
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
