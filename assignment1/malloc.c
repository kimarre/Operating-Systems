#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// TODO: Handle if there's no memory at all for sbrk to take from

#define CHUNK_SIZE 65536
#define HEADER_SIZE 16

typedef struct Block {
   struct Block *next;
   int isTaken;
   int size;
} Block;

static int chunkRemaining = -1;
static Block *firstBlock = NULL;
static Block *lastBlock = NULL;

void initNewBlock(Block *lastBlock, Block *prevBlock, int size) {
   if (chunkRemaining < size + HEADER_SIZE) {
      // if we need more chunk space
      puts("making a new chunk");
      sbrk((intptr_t)CHUNK_SIZE);
      chunkRemaining += CHUNK_SIZE;
   }

   chunkRemaining -= size + sizeof(Block);

   if (prevBlock) {
      prevBlock->next = lastBlock;
   }

   lastBlock->isTaken = 1;
   lastBlock->size = size;
   lastBlock->next = NULL;
}

/* Looks for a block big enough, consolidating blocks in the process.
 * Returns 0 if nothing is currently available or a ptr to the new block for
 * use.
 */
Block *findExistingBlock(int size) {
   if (!firstBlock) {
      // If we don't have a chunk yet
      return NULL;
   }

   Block *temp = firstBlock;
   int addedSize;

   while (temp) {
      puts("loop");
      if (temp->isTaken == 0) {
         // this is only size, not size + HEADER_SIZE, because these are
         // existing blocks that already have headers
         if (temp->size >= size) {
            puts("found a single block large enough");
            // If we find a block that's big enough
            temp->isTaken = 1;
            return temp;

         } else if (temp->next && temp->next->isTaken == 0) {
            // The next block is free
            addedSize = temp->size + temp->next->size + HEADER_SIZE;

            if (addedSize >= size) {
               puts("combined sizes of two is enough");
               // The combined sizes of the two is enough

               // Remove the eaten up one from the linked list
               temp->next = temp->next->next;
               temp->size = addedSize;
               temp->isTaken = 1;

               return temp;
            }
         }
      }
      temp = temp->next;
   }
   return NULL;
}

void *realloc(void *ptr, size_t size) {
   if (ptr == NULL) {
      return malloc(size);
   }
   Block *prevBlock;
   Block *orig = ptr - HEADER_SIZE;

   // CHECK TO SEE IF IT"S ALREADY ENOUGH SPACE?

   if (orig->next && orig->next->isTaken == 0) {
      int combinedSize = orig->size + orig->next->size + HEADER_SIZE;
      // add HEADER_SIZE because one of the old ones is space we can reuse

      if (combinedSize >= size) {
         puts("realloc combining two blocks");
         // Combine the two blocks
         orig->next = orig->next->next;
         orig->size = combinedSize;
         return ptr;
      }
   }

   Block *newBlock = findExistingBlock(size);

   if (newBlock) {
      // there's an existing block that could be used
      puts("realloc found an existing block to use");
      orig->isTaken = 0;
      return memcpy(newBlock + 1, ptr, orig->size);
   } else {
      prevBlock = lastBlock;
      lastBlock += lastBlock->size + 1;
      initNewBlock(lastBlock, prevBlock, size);
      puts("realloc creating new block");

      return memcpy(lastBlock + HEADER_SIZE, ptr, orig->size) + 1;
   }
}

void *malloc(size_t size) {
   Block *prevBlock;

   if (chunkRemaining == -1) {
      // A chunk doesn't exist yet
      firstBlock = lastBlock = (Block *)sbrk((intptr_t)CHUNK_SIZE);
      chunkRemaining = CHUNK_SIZE;
      initNewBlock(lastBlock, NULL, size);

      return lastBlock + 1;
   } else {
      // If there's an old, freed block available for use
      Block *blockFound = findExistingBlock(size);

      if (blockFound) {
         puts("found an existing block to return");
         return blockFound;
      }
      puts("didn't find a freed block");

      prevBlock = lastBlock;
      lastBlock += lastBlock->size + 1;
      initNewBlock(lastBlock, prevBlock, size);

      return lastBlock + 1;
   }

   return 0;
}

void free(void *ptr) {
   Block *header = ptr - HEADER_SIZE;

   if (!ptr) {
      return;
   } else {
      puts("set isTaken to 0");
      header->isTaken = 0;
   }
}

void *calloc(size_t num, size_t size) {
   void *ptr = malloc(num*size);
   memset(ptr, 0, num*size);

   return ptr;
}
