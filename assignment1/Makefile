CC=gcc
LD=gcc
CFLAGS=-g -Wall
LDFLAGS=-g

test: tryme.o
	$(CC) $(CFLAGS) -c -o tryme.o tryme.c
	$(LD) -L lib64 -lmalloc -o tryme tryme.o

intel-all: lib/libmalloc.so lib64/libmalloc.so

lib/libmalloc.so: lib malloc32.o
	$(CC) $(CFLAGS) -fpic -m32 -shared -o $@ malloc32.o

lib64/libmalloc.so: lib64 malloc64.o
	$(CC) $(CFLAGS) -fpic -shared -o $@ malloc64.o

lib:
	mkdir lib

lib64:
	mkdir lib64

malloc32.o: malloc.c
	$(CC) $(CFLAGS) -fpic -m32 -c -o malloc32.o malloc.c

malloc64.o: malloc.c
	$(CC) $(CFLAGS) -fpic -c -o malloc64.o malloc.c

malloc: testing.o malloc.o
	$(LD) $(LDFLAGS) testing.o malloc.o -o testing

testing.o: testing.c
	$(CC) $(CFLAGS) -c testing.c

tryme.o: tryme.c
	$(CC) $(CFLAGS) -c tryme.c

malloc.o: malloc.c
	$(CC) $(CFLAGS) -c malloc.c

debug:
	lldb testing

clean:
	rm testing.o malloc.o testing tryme.o malloc64.o malloc32.o
	rm -r lib lib64

upload:
	rsync Makefile functions.c malloc.c testing.c unix3:~/OS/assignment1
