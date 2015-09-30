#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE = 65536;
static break = NULL;
static lastNode = NULL;

typedef struct Block {
   struct Block *next;
   int isTaken;
   int size;
} Block;

void *malloc(size_t size) {
   // if: we've used sbrk before and have a chunk
   //    if: there's a free already made block to use
   //       use it
   //    else: 
   //       if: there's more room in the chunk
   //          make a new block
   //       else:
   //          make a new chunk
   //          make a new block

   char *str = "In the new malloc";
   int blockSize = sizeof(Block);
   
   return 0;
}

