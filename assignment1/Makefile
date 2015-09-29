CC=gcc
LD=gcc
CFLAGS=-Wall -g -pedantic
LDFLAGS=-g

test: testing.o functions.o
	$(LD) $(LDFLAGS) testing.o functions.o -o testing

testing.o: testing.c
	$(CC) $(CFLAGS) -c testing.c

functions.o: functions.c
	$(CC) $(CFLAGS) -c functions.c
