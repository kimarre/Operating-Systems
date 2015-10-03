#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#define CHUNK_SIZE 65536

typedef struct Block {
   struct Block *next;
   int isTaken;
   int size;
} Block;

static int headerSize = -1;
static int chunkRemaining = -1;
static Block *firstBlock = NULL;
static Block *lastBlock = NULL;

/* Calls sbrk and verifies that it succeeds. Exits if it doesn't.
 */
void *moarChunk(int increment) {
   void *result = sbrk((intptr_t)CHUNK_SIZE);
   if (result == (void *)(-1)) {
      errno = ENOMEM;
      perror("Ran out of memory to allocate with sbrk");
      exit(0);
   }
   return result;
}

/* Makes sure num is divisible by 16, or returns a value that rounds up to one
 */
int getAlignedVal(int num) {
   if (num % 16 == 0) {
      return num;
   } else {
      int toAdd = 16 - (num % 16);
      return num + toAdd;
   }

}

void *getAlignedPtr(void *ptr) {
   int temp = (int)ptr;

   if (temp % 16 == 0) {
      return ptr;
   } else {
      int toAdd = 16 - (temp % 16);
      return (char *)ptr + toAdd;
   }
}

/* For use when findExitingBlock() returns 0. Handles chunkRemaining and sets
 * up the new block at lastBlock.
 */
void initNewBlock(Block *lastBlock, Block *prevBlock, int size) {
   int toAllocate;

   if (size > CHUNK_SIZE) {
      toAllocate = size - chunkRemaining + sizeof(Block);
      toAllocate = getAlignedVal(toAllocate);

      moarChunk((intptr_t)toAllocate);
      chunkRemaining += toAllocate;
   }

   if (chunkRemaining < size + headerSize) {
      // if we need more chunk space
      moarChunk((intptr_t)CHUNK_SIZE);

      chunkRemaining += CHUNK_SIZE;
   }

   int totalSpace = getAlignedVal(size + sizeof(Block));
   chunkRemaining -= totalSpace;

   if (size > CHUNK_SIZE) {
      assert(chunkRemaining == 0);
   }

   if (prevBlock) {
      prevBlock->next = lastBlock;
   }

   lastBlock->isTaken = 1;
   lastBlock->size = totalSpace - headerSize;
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
      if (temp->isTaken == 0) {
         // this is only size, not size + headerSize, because these are
         // existing blocks that already have headers
         if (temp->size >= size) {
            // If we find a block that's big enough
            temp->isTaken = 1;
            return temp;

         } else if (temp->next && temp->next->isTaken == 0) {
            // The next block is free
            addedSize = temp->size + temp->next->size + headerSize;

            if (addedSize >= size) {
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
   void *debug_ptr = ptr;
   if (ptr == NULL) {
      return malloc(size);
   }
   Block *prevBlock = NULL;
   Block *orig = (Block *)ptr - 1;

   // CHECK TO SEE IF IT"S ALREADY ENOUGH SPACE?
   if (size <= orig->size) {
      return ptr;
   }


   // If the block after the original one we're given is free
   if (orig->next && orig->next->isTaken == 0) {
      int combinedSize = orig->size + orig->next->size + headerSize;
      // add headerSize because one of the old ones is space we can reuse

      if (combinedSize >= size) {
         // Combine the two blocks
         orig->next = orig->next->next;
         orig->size = combinedSize;

         if (getenv("DEBUG_MALLOC")) {
            char buffer[100];
            char *debug = "MALLOC: realloc(%p, %d) => (ptr=%p, size=%d)\n";
            snprintf(buffer, 100, debug, debug_ptr, size, ptr, size);
            fputs(buffer, stderr);
         }

         return orig+1;
      }
   }

   Block *newBlock = findExistingBlock(size);

   if (newBlock) {
      // there's an existing block that could be used
      orig->isTaken = 0;

      if (getenv("DEBUG_MALLOC")) {
         char buffer[100];
         char *debug = "MALLOC: realloc(%p, %d) => (ptr=%p, size=%d)\n";
         snprintf(buffer, 100, debug, debug_ptr, size, newBlock + 1, size);
         fputs(buffer, stderr);
      }

      return memcpy(newBlock + 1, ptr, orig->size);
   } else {
      prevBlock = lastBlock;
      lastBlock += lastBlock->size + 1;
      initNewBlock(lastBlock, prevBlock, size);

      if (getenv("DEBUG_MALLOC")) {
         char buffer[100];
         char *debug = "MALLOC: realloc(%p, %d) => (ptr=%p, size=%d)\n";
         snprintf(buffer, 100, debug, debug_ptr, size, lastBlock + headerSize,
          size);
         fputs(buffer, stderr);
      }

      return memcpy(lastBlock + headerSize, ptr, orig->size) + 1;
   }
}

void *malloc(size_t size) {
   Block *prevBlock = NULL;


   // First time calling malloc, set headersize
   if (headerSize < 0) {
      int blockSize = sizeof(Block);

      headerSize = getAlignedVal(blockSize);
   }

   if (chunkRemaining == -1) {
      // A chunk doesn't exist yet
      Block *breakPtr = (Block *)moarChunk((intptr_t)CHUNK_SIZE);

      //breakPtr = (Block *)getAlignedVal((int)breakPtr);
      breakPtr = getAlignedPtr(breakPtr);

      firstBlock = lastBlock = breakPtr;
      chunkRemaining = CHUNK_SIZE;
      initNewBlock(lastBlock, NULL, size);

      if (getenv("DEBUG_MALLOC")) {
         char buffer[100];
         char *debug = "MALLOC: malloc(%d) => (ptr=%p, size=%d)\n";
         snprintf(buffer, 100, debug, size, lastBlock + 1,
          size);
         fputs(buffer, stderr);
      }

      return lastBlock + 1;
   } else {
      // look for an old, freed block available for use
      Block *blockFound = findExistingBlock(size);

      if (blockFound) {
         if (getenv("DEBUG_MALLOC")) {
            char buffer[100];
            char *debug = "MALLOC: malloc(%d) => (ptr=%p, size=%d)\n";
            snprintf(buffer, 100, debug, size, blockFound,
             size);
            fputs(buffer, stderr);
         }

         return blockFound;
      }

      prevBlock = lastBlock;
      lastBlock += lastBlock->size + 1;
      initNewBlock(lastBlock, prevBlock, size);

      if (getenv("DEBUG_MALLOC")) {
         char buffer[100];
         char *debug = "MALLOC: malloc(%d) => (ptr=%p, size=%d)\n";
         snprintf(buffer, 100, debug, size, lastBlock + 1, size);
         fputs(buffer, stderr);
      }

      return lastBlock + 1;
   }

   return 0;
}

void free(void *ptr) {
   if (ptr == NULL) {
      return;
   }

   Block *header = ptr - headerSize;

   if (!ptr) {
      if (getenv("DEBUG_MALLOC")) {
         char buffer[100];
         char *debug = "MALLOC: free(%p)\n";
         snprintf(buffer, 100, debug, NULL);
         fputs(buffer, stderr);
      }
      return;
   } else {
      header->isTaken = 0;
   }
}

void *calloc(size_t num, size_t size) {
   void *ptr = malloc(num*size);
   memset(ptr, 0, num*size);

   if (getenv("DEBUG_MALLOC")) {
      char buffer[100];
      char *debug = "MALLOC: calloc(%d, %d) => (ptr=%p, size=%d)\n";
      snprintf(buffer, 100, debug, num, size, ptr, size);
      fputs(buffer, stderr);
   }

   return ptr;
}
