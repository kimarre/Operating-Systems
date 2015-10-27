/*
 * TODO:
 *    - use dawdle() instead of sleep()
 *    - print what forks they're holding
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define EATING 1
#define THINKING 2
#define TRANSITION 3

#define NUM_PHILOSOPHERS 5

typedef struct Philosopher {
   int id;
   /* indices into the forks array */
   int left;
   int isHoldingLeft;
   int right;
   int isHoldingRight;
   int state;
   int repeat;
} Philosopher;

static pthread_mutex_t forks[NUM_PHILOSOPHERS];
static Philosopher philosophers[NUM_PHILOSOPHERS];
static pthread_t threads[NUM_PHILOSOPHERS];
static pthread_mutex_t printLock;

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
      int i, state;
      for(i = 0; i < NUM_PHILOSOPHERS; i++) {
         state = philosophers[i].state;

         switch(state) {
            case TRANSITION:
               printf("|       ");
               break;
            case EATING:
               printf("|  EAT  ");
               break;
            case THINKING:
               printf("| THINK ");
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
      philosophers[i].isHoldingLeft = 0;
      philosophers[i].isHoldingRight = 0;

      if (i == NUM_PHILOSOPHERS - 1) {
         /* if it's the last */
         philosophers[i].left = 0;
      } else {
         philosophers[i].left = i + 1;
      }
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
         if (result) {
            perror("lock failed :(\n");
            exit(1);
         }
         phil->isHoldingRight = 1;

         result = pthread_mutex_lock(&forks[phil->left]);
         if (result) {
            perror("lock failed :(\n");
            exit(1);
         }
         phil->isHoldingLeft = 1;
      } else {
         /* odd philosopher - reach for left fork first */
         result = pthread_mutex_lock(&forks[phil->left]);
         if (result) {
            perror("lock failed :(\n");
            exit(1);
         }
         phil->isHoldingLeft = 1;

         result = pthread_mutex_lock(&forks[phil->right]);
         if (result) {
            perror("lock failed :(\n");
            exit(1);
         }
         phil->isHoldingRight = 1;
      }

      /* eat */
      phil->state = EATING;
      printStatus(NULL);
      sleep(1);

      /* transition */
      phil->state = TRANSITION;
      printStatus(NULL);

      result = pthread_mutex_unlock(&forks[phil->right]);
      if (result) {
         perror("lock failed :(\n");
         exit(1);
      }
      phil->isHoldingRight = 0;

      result = pthread_mutex_unlock(&forks[phil->left]);
      if (result) {
         perror("lock failed :(\n");
         exit(1);
      }
      phil->isHoldingLeft = 0;

      /* think */
      phil->state = THINKING;
      printStatus(NULL);
      sleep(1);
   }

   phil->state = TRANSITION;

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
   printf("|==========|==========|==========|==========|==========|\n");
   printf("|    A     |     B    |     C    |     D    |     E    |\n");
   printf("|==========|==========|==========|==========|==========|\n");

   int i;
   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      result = pthread_create(&threads[i], NULL, cycle, (void *)(&philosophers[i]));
      if (result) {
         perror("Thread creation failed\n");
         exit(1);
      }
   }

   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      pthread_join(threads[i], NULL);
   }
   printf("|==========|==========|==========|==========|==========|\n");

   return 0;
}
