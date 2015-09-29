CC=gcc
LD=gcc
CFLAGS=-Wall -g -pedantic
LDFLAGS=-g

pipeit: pipeit.o
	$(LD) $(LDFLAGS) pipeit.o -o pipeit

pipeit.o: pipeit.c
	$(CC) $(CFLAGS) -c pipeit.c
