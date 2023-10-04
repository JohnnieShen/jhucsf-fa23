/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */

#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

    .section .rodata
    sSpace: .string " "
    sTab: .string "\t"
    sLine: .string "\n"
    sR: .string "\r"
    sF: .string "\f"
    sV: .string "\v"
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:
	movq $5381, %rax
.hash_loop:
    movzbq (%rdi), %r8
    cmp $0, %r8
    je .hash_loop_done
    imulq $33, %rax
    addq %r8, %rax
    inc %rdi
    jmp .hash_loop

.hash_loop_done:
	ret

/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare:
    //allocate the memory
    subq $8, %rsp

    //initialize the two strings to %rax and %rcx for comparison
    movq %rdi, %rax
    movq %rsi, %rcx
.str_compare_loop:
    movzbl (%rax), %edx
    movzbl (%rcx), %ebx

    //compare null to the left side
    cmp $0, %edx
    je .LeftNull

    //if left is not null and right is null, right must be smaller
    cmp $0, %ebx
    je .LeftGreater

    cmp %edx, %ebx
    je .nextChar

    //if ebx is less than edx, the right hand side is smaller, we return 1
    cmp %edx, %ebx
    jl .LeftGreater

    //if edx is greater than ebx, the left hand side is smaller, we return -1
    cmp %edx, %ebx
    jg .RightGreater

.nextChar:
    inc %rax
    inc %rcx
    //loop again
    jmp .str_compare_loop


.LeftNull:
    //if right side is null as well, it is equal, otherwise left is smaller
    cmp $0, %ebx
    je .Equal
    jmp .RightGreater

.LeftGreater:
    movl $1, %eax
    addq $8, %rsp
    ret

.RightGreater:
    movl $-1, %eax
    addq $8, %rsp
    ret

.Equal:
    movl $0, %eax
    addq $8, %rsp
    ret

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
.str_copy_loop:
    movzbq (%rsi), %r8 //move the current char of source into %r8 
    test %r8b, %r8b //check if the char is a null
    je .str_copy_loop_done //jump to loop done
    mov %r8b, (%rdi) //move the current char to dest
    inc %rdi //advance in the strings
    inc %rsi
    jmp .str_copy_loop 
.str_copy_loop_done:
    movq $0, (%rdi) //set the last character of dest to \0
	ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
    subq $8, %rsp
    //get the input parameter, often stored in the %dil pointer
    cmp %dil, sSpace //check if it is equal to ' '
    je .isSpace
    cmp %dil, sTab //check if it is equal to '\t'
    je .isSpace
    cmp %dil, sLine //check if it is equal to '\n'
    je .isSpace
    cmp %dil, sF //check if it is equal to '\f'
    je .isSpace
    cmp %dil, sR //check if it is equal to '\r'
    je .isSpace
    cmp %dil, sV //check if it is equal to '\v'
    je .isSpace

    //otherwise it is not a space, return 0 and free the memory
    movl $0, %eax
    addq $8, %rsp
    ret

.isSpace:
    //if it is space, return 1 and free the memory
   movl $1, %eax
   addq $8, %rsp
   ret


/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
    //allocate memory for the function
    subq $8, %rsp

    //we first compare the value of the character, if it is greater equal to 'a'. It must be a lowercase character or not a
    //character, we then check if it's less equal to 'z'
    cmpb $'a', %dil
    jge .LowerCaseLetter
    //jump to check if it's lowercase letter or not

    //we reach here is the character is less than 'a'. It must be an uppercase letter or not an letter. We check if it is
    //in between 'A' and 'Z'
    cmpb $'A', %dil
    jge .UpperCaseLetter

    //return 0 and free back the memory if it is less than 'A', meaning it is not a character
    movl $0, %eax
    addq $8, %rsp
    ret


.UpperCaseLetter:
    //we compare if it is an uppercase letter. If it is less equal to 'Z', this means it is alphabetical
    cmpb $'Z', %dil
    jle .IsAlpha

    //return 0 and free back the memory if it is greater than 'Z', meaning it is not in the alphabet
    movl $0, %eax
    addq $8, %rsp
    ret

.LowerCaseLetter:
    //we compare if it is an uppercase letter. If it is less equal to 'z', this means it is alphabetical
    cmpb $'z', %dil
    jle .IsAlpha

    //return 0 and free back the memory if it is greater than 'z', meaning it is not in the alphabet
    movl $0, %eax
    addq $8, %rsp
    ret

.IsAlpha:
    //return 1 and free back the memory if it is alphabetical
    movl $1, %eax
    addq $8, %rsp
    ret


/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
    pushq %r12
	pushq %r13
    pushq %rbp
    pushq %rbx
    movq $0, %r13
    movq %rdi, %rbp
    movq %rsi, %rbx
    cmpq $0, %rdi
    je .readnext_0_character_word
.readnext_loop_first_character:
    call fgetc
    movl %eax, %r12d
    cmpl $-1, %r12d //check if the first character is EOF
    je .readnext_loop_done //terminate the function
    pushq %rdi
    movq %rax, %rdi
    call wc_isspace //check if the first character is space
    popq %rdi
    cmpl $1, %eax
    je .readnext_loop_first_character //restart loop until the first character is space
    movl %r12d, (%rbx) //put the character into array
    inc %rbx //increase array pointer
    inc %r13 //increase word length counter
.readnext_loop_read_word:
    cmpq $MAX_WORDLEN, %r13
    jge .readnext_loop_word_end //reached length limit
    movq %rbp, %rdi
    call fgetc
    movl %eax, %r12d
    cmpl $-1, %r12d
    je .readnext_loop_word_end //word is completely read, reached EOF
    pushq %rdi
    movq %rax, %rdi
    call wc_isspace
    popq %rdi
    cmpl $1, %eax
    je .readnext_loop_word_end //word is completely read, reached space
    movl %r12d, (%rbx)
    inc %rbx
    inc %r13
    jmp .readnext_loop_read_word 
.readnext_loop_word_end:
    movb $0, (%rbx) //add null terminator
.readnext_loop_done:
    cmpq $0, %r13
    je .readnext_0_character_word
    movq $1, %rax
    jmp .readnext_end
.readnext_0_character_word:
    movq $0, %rax
.readnext_end:
    popq %rbx
    popq %rbp
    popq %r13
    popq %r12
	ret 

/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
.tolower_loop:
    movzbq (%rdi), %r8
    cmpq $0, %r8
    je .tolower_loop_done
    cmpq $65, %r8
	jl .tolower_loop_advance
	cmpq $90, %r8
	jg .tolower_loop_advance
    addq $32, %r8
	movb %r8b, (%rdi)

.tolower_loop_advance:
	incq %rdi
	jmp .tolower_loop

.tolower_loop_done:
	ret

/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	/* TODO: implement */
	ret

/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	/* TODO: implement */
	ret

/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
    subq $8, %rsp
	cmpq $0, %rdi
    je .free_chain_loop_done
.free_chain_loop:
    movq (%rdi), %rax
    movq WORDENTRY_NEXT_OFFSET(%rdi), %r8
    call free
    movq %r8, %rdi
    cmpq $0, %rdi
    je .free_chain_loop
.free_chain_loop_done:
    addq $8, %rsp
	ret

/*
vim:ft=gas:
*/
