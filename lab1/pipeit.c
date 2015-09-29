#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

int main() {
   int fd[2];
   int firstChildPID, secondChildPID;
   int status = 0;
   int i = 2;

   pipe(fd);

   /* first child - to run ls */
   if ((firstChildPID = fork()) == 0) {
      dup2(fd[1], 1);
      close(fd[0]);
      close(fd[1]);

      execl("/bin/ls", "-a", NULL);
      perror("Failed to exec ls");
      exit(1);
   } else if (firstChildPID < 0) {
      perror("Forking for ls failed");
      exit(1);
   } else {
      /* second child - to run sort and output to file */
      if ((secondChildPID = fork()) == 0) {
         int openfd = open("outfile", O_WRONLY | O_CREAT, 
          O_TRUNC | S_IWRITE | S_IREAD); 

         if (openfd < 0) {
            perror("Could not open a file for writing");
            exit(1);
         }

         dup2(fd[0], 0);
         dup2(openfd, 1);
         close(fd[1]);

         /* calling this eats up the first argument, so we have to give it 
          * again */
         execlp("sort", "sort", "-r", NULL);
         perror("Failed to exec sort");
         exit(1);
      } else if (secondChildPID < 0) {
         /* Fork failed */
         perror("Forking for sort failed");
         exit(1);
      }
   }

   /* Close the pipe for parent so it doesn't interfere with children */
   close(fd[0]);
   close(fd[1]);

   /* Wait and catch the child exit codes */
   while (i--) {
      wait(&status);
      if (status != 0) {
         exit(1);
      }
   }

   return 0;
}
