CC = gcc
CFLAGS = -g -no-pie

# Executables to build
EXES = hello countTo10 countToN countToNIncr

% : %.S
	$(CC) $(CFLAGS) $*.S -o $@

all : $(EXES)

clean :
	rm -f $(EXES)
