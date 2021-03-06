#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define EATING 1
#define THINKING 2
#define TRANSITION 3

#define NUM_PHILOSOPHERS 5
#define CHAR_OFFSET 48

void dawdle();

typedef struct Philosopher {
   int id;
   int left;     /* indices into the forks array */
   int right;
   int state;
   int repeat;
   char forksHeld[NUM_PHILOSOPHERS];
} Philosopher;

static pthread_mutex_t forks[NUM_PHILOSOPHERS];
static Philosopher philosophers[NUM_PHILOSOPHERS];
static pthread_t threads[NUM_PHILOSOPHERS];
static pthread_mutex_t printLock;

char intToChar(int num) {
   return (char)(num + CHAR_OFFSET);
}

/* Given NULL, it will print the statuses of all threads in the table
 * Given something else, it'll print the string passed - for debugging
 */
void printStatus(char *string) {
   int result;
   result = pthread_mutex_lock(&printLock);
   if (result) {
      perror("lock failed :(\n");
      exit(1);
   }

   if (string) {
      printf(string);
   } else {
      int i, j, state;
      for(i = 0; i < NUM_PHILOSOPHERS; i++) {
         state = philosophers[i].state;

         switch(state) {
            case TRANSITION:
               printf("| ");
               for (j = 0; j < NUM_PHILOSOPHERS; j++) {
                  printf("%c", philosophers[i].forksHeld[j]);
               }
               printf("       ");
               break;
            case EATING:
               printf("| ");
               for (j = 0; j < NUM_PHILOSOPHERS; j++) {
                  printf("%c", philosophers[i].forksHeld[j]);
               }
               printf("  EAT  ");
               break;
            case THINKING:
               printf("| ");
               for (j = 0; j < NUM_PHILOSOPHERS; j++) {
                  printf("%c", philosophers[i].forksHeld[j]);
               }
               printf(" THINK ");
         }
      }
      printf("|\n");

   }

   result = pthread_mutex_unlock(&printLock);
   if (result) {
      perror("lock failed :(\n");
      exit(1);
   }
}

void init(int numRepeat) {
   int result;
   int i;

   result = pthread_mutex_init(&printLock, NULL);
   if (result) {
      perror("Failed to init printLock\n");
   }

   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      /* initialize the fork mutices */
      result = pthread_mutex_init(&forks[i], NULL);

      /* check that init succeeded */
      if (result) { /* returns 0 on success */
         perror("Failed trying to init a mutex.\n");
         exit(1);
      }

      /* initialize philosophers */
      philosophers[i].id = i;
      philosophers[i].right = i;
      philosophers[i].repeat = numRepeat;
      philosophers[i].state = TRANSITION;

      int j;
      for (j = 0; j < NUM_PHILOSOPHERS; j++) {
         philosophers[i].forksHeld[j] = '-';
      }

      if (i == NUM_PHILOSOPHERS - 1) {
         /* if it's the last */
         philosophers[i].left = 0;
      } else {
         philosophers[i].left = i + 1;
      }
   }
}

void testLockResult(int result) {
   if (result) {
      perror("Lock/unlock failed :(\n");
      exit(1);
   }
}

void *cycle(void *arg) {
   Philosopher *phil = (Philosopher *)arg;
   int result;

   int repeat;
   for (repeat = 0; repeat < phil->repeat; repeat++) {
      printStatus(NULL);

      /* transition - pick up forks */
      phil->state = TRANSITION;
      if (phil->id % 2 == 0) { /* even philosopher, reach for right fork */
         result = pthread_mutex_lock(&forks[phil->right]);
         testLockResult(result);
         phil->forksHeld[phil->right] = intToChar(phil->right);

         printStatus(NULL);

         result = pthread_mutex_lock(&forks[phil->left]);
         testLockResult(result);

         phil->forksHeld[phil->left] = intToChar(phil->left);
         printStatus(NULL);
      } else {
         /* odd philosopher - reach for left fork first */
         result = pthread_mutex_lock(&forks[phil->left]);
         testLockResult(result);

         phil->forksHeld[phil->left] = intToChar(phil->left);
         printStatus(NULL);

         result = pthread_mutex_lock(&forks[phil->right]);
         testLockResult(result);

         phil->forksHeld[phil->right] = intToChar(phil->right);
         printStatus(NULL);
      }

      /* eat */
      phil->state = EATING;
      printStatus(NULL);
      dawdle();

      /* transition */
      phil->state = TRANSITION;
      printStatus(NULL);

      phil->forksHeld[phil->right] = '-';
      result = pthread_mutex_unlock(&forks[phil->right]);
      testLockResult(result);

      printStatus(NULL);

      phil->forksHeld[phil->left] = '-';
      result = pthread_mutex_unlock(&forks[phil->left]);
      testLockResult(result);

      printStatus(NULL);

      /* think */
      phil->state = THINKING;
      printStatus(NULL);
      dawdle();
   }

   phil->state = TRANSITION;
   printStatus(NULL);

   pthread_exit(0);
}

int main(int argc, const char *argv[]) {
   int result;
   int repetitions = 1;

   if (argc > 1) {
      /* if there were more arguments than just the executable */
      repetitions = strtol(argv[1], NULL, 10);
   }

   init(repetitions);

   /* print initial table labels */
   printf("|=============|=============|=============|=============|");
   printf("=============|\n");
   printf("|      A      |      B      |      C      |      D      |");
   printf("      E      |\n");
   printf("|=============|=============|=============|=============|");
   printf("=============|\n");

   int i;
   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      result = pthread_create(&threads[i], NULL, cycle,
       (void *)(&philosophers[i]));
      if (result) {
         perror("Thread creation failed\n");
         exit(1);
      }
   }

   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      pthread_join(threads[i], NULL);
   }
   printf("|=============|=============|=============|=============|");
   printf("=============|\n");

   return 0;
}
