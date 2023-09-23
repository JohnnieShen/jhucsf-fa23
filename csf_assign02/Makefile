# CSF Assignment 2 Makefile
# You should not need to change anything here

CC = gcc
CFLAGS = -g -Wall -std=gnu11 -no-pie
ASMFLAGS = -g -no-pie
LDFLAGS = -no-pie

C_SRCS = wctests.c tctest.c c_wcfuncs.c c_wcmain.c
ASM_SRCS = asm_wcfuncs.S asm_wcmain.S

C_WCTESTS_OBJS = wctests.o c_wcfuncs.o tctest.o
C_WORDCOUNT_OBJS = c_wcmain.o c_wcfuncs.o

ASM_WCTESTS_OBJS = wctests.o asm_wcfuncs.o tctest.o
ASM_WORDCOUNT_OBJS = asm_wcmain.o asm_wcfuncs.o

CASM_WORDCOUNT_OBJS = c_wcmain.o asm_wcfuncs.o

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.o : %.S
	$(CC) $(ASMFLAGS) -c $*.S -o $*.o

all : c_wctests c_wordcount

c_wctests : $(C_WCTESTS_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(C_WCTESTS_OBJS)

c_wordcount : $(C_WORDCOUNT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(C_WORDCOUNT_OBJS)

asm_wctests : $(ASM_WCTESTS_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(ASM_WCTESTS_OBJS)

asm_wordcount : $(ASM_WORDCOUNT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(ASM_WORDCOUNT_OBJS)

# casm_wordcount is the wordcount program linked with the C
# main function but the assembly-language function implementations
casm_wordcount : $(CASM_WORDCOUNT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(CASM_WORDCOUNT_OBJS)

clean :
	rm -f *.o depend.mak

depend :
	$(CC) $(CFLAGS) -M $(C_SRCS) $(ASM_SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak
