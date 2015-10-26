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
   int right;
   int state;
   int repeat;
} Philosopher;


/*              A
 *          0        1
 *       E              B
 *         4             2
 *            D   3   C
 *
 *
 *
 */

static pthread_mutex_t forks[NUM_PHILOSOPHERS];
static Philosopher philosophers[NUM_PHILOSOPHERS];
static pthread_t threads[NUM_PHILOSOPHERS];

void init(int numRepeat) {
   int result;
   int i;

   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      /* initialize the fork mutices */
      result = pthread_mutex_init(&forks[i], NULL);

      /* check that init succeeded */
      if (result) { /* returns 0 on success */
         perror("Failed trying to init a mutex.");
         exit(1);
      }

      /* initialize philosophers */
      philosophers[i].id = i;
      philosophers[i].right = i;
      philosophers[i].repeat = numRepeat;
      philosophers[i]. state = TRANSITION;

      if (i == NUM_PHILOSOPHERS - 1) {
         /* if it's the last */
         philosophers[i].left = 0;
      } else {
         philosophers[i].left = i + 1;
      }
   }
}

void cycle(Philosopher phil) {
   /* */
}

int main(int argc, const char *argv[]) {
   int repetitions = 0;
   if (argc > 1) {
      /* if there were more arguments than just the executable */
      //if (typeof(argv[1]) == int) {
         repetitions = argv[1];
      //}

   }
   init(repetitions);

   int i;
   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      create(treads[i], NULL, cycle, philosophers[i]);
   }

   return 0;
}
