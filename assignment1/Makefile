CC=gcc
LD=gcc
CFLAGS=-g -Wall
LDFLAGS=-g

test:
	$(CC) $(CFLAGS) -c -o testing.o testing.c
	$(LD) -L lib -lmalloc -o testing testing.o

intel-all: malloc.c
	mkdir lib
	$(CC) $(CFLAGS) -fpic -m32 -c -o malloc32.o malloc.c
	$(CC) $(CFLAGS) -fpic -m32 -shared -o lib/libmalloc.so malloc32.o
	mkdir lib64
	$(CC) $(CFLAGS) -fpic -c -o malloc64.o malloc.c
	$(CC) $(CFLAGS) -fpic -shared -o lib64/libmalloc.so malloc64.o

malloc: testing.o malloc.o
	$(LD) $(LDFLAGS) testing.o malloc.o -o testing

testing.o: testing.c
	$(CC) $(CFLAGS) -c testing.c

malloc.o: malloc.c
	$(CC) $(CFLAGS) -c malloc.c

debug:
	lldb testing

clean:
	rm testing.o malloc.o testing

upload:
	rsync Makefile functions.c malloc.c testing.c unix3:~/OS/assignment1
