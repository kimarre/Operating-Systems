CC=gcc
LD=gcc
CFLAGS=-g
LDFLAGS=-g

malloc: testing.o malloc.o
	$(LD) $(LDFLAGS) testing.o malloc.o -o testing

testing.o: testing.c
	$(CC) $(CFLAGS) -c testing.c

malloc.o: malloc.c
	$(CC) $(CFLAGS) -c malloc.c

debug:
	lldb testing

clean:
	rm testing.o testing functions.o
