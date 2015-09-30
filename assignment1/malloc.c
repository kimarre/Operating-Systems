#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// TODO: Handle if there's no memory at all for sbrk to take from

#define CHUNK_SIZE 65536
#define HEADER_SIZE 16

typedef struct Block {
   struct Block *next;
   int isTaken;
   int size;
} Block;

static int chunkRemaining = CHUNK_SIZE;
static Block *lastBlock = NULL;

void initNewBlock(Block *lastBlock, Block *prevBlock, int size) {
   chunkRemaining -= size + sizeof(Block);

   if (prevBlock) {
      prevBlock->next = lastBlock;
   } else {
      prevBlock = NULL;
   }

   lastBlock->isTaken = 1;
   lastBlock->size = size;
}

void *realloc(void *ptr, size_t size) {

}

void *malloc(size_t size) {
   Block *prevBlock;
   // [x] if: we've used sbrk before and have a chunk
   // [ ]    if: there's a free already made block to use
   // [ ]       use it
   // [ ]    else: 
   // [x]       if: there's more room in the chunk
   // [x]          make a new block
   // [x]       else:
   // [x]          make a new chunk
   // [x] else:
   // [x]    use sbrk to make our first chunk


   if (!lastBlock) {
      // A chunk doesn't exist yet
      lastBlock = (Block *)sbrk((intptr_t)CHUNK_SIZE);
      initNewBlock(lastBlock, NULL, size);

      return lastBlock + 1;
   } else {
      // ### do this later! if there's an already made block that's been freed

      if (chunkRemaining < size + HEADER_SIZE) {
         // if we need more chunk space
         sbrk((intptr_t)CHUNK_SIZE);
         chunkRemaining += CHUNK_SIZE;
      }

      prevBlock = lastBlock;
      lastBlock += lastBlock->size + 1;
      initNewBlock(lastBlock, prevBlock, size);

      return lastBlock + 1;
   }

   return 0;
}

void free(void *ptr) {
   if (!ptr) {
      return;
   } else {
      ptr->isTaken = 0;
   }
}

void *calloc(size_t num, size_t size) {
   void *ptr = malloc(num*size);
   memset(ptr, 0, num*size);

   return toReturn;
}
