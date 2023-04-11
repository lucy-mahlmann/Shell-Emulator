#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


const int MAX = 13;


static void doFib(int n, int doPrint);


/*
* unix_error - unix-style error routine.
*/
inline static void unix_error(char *msg) {
 fprintf(stdout, "%s: %s\n", msg, strerror(errno));
 exit(1);
}


int main(int argc, char **argv) {
 int arg;
 int print = 1;


 if (argc != 2) {
   fprintf(stderr, "Usage: fib <num>\n");
   exit(-1);
 }


 arg = atoi(argv[1]);
 if (arg < 0 || arg > MAX) {
   fprintf(stderr, "number must be between 0 and %d\n", MAX);
   exit(-1);
 }


 doFib(arg, print);


 return 0;
}


/*
* Recursively compute the specified number. If print is
* true, print it. Otherwise, provide it to my parent process.
*
* NOTE: The solution must be recursive and it must fork
* a new child for each call. Each process should call
* doFib() exactly once.
*/
static void doFib(int n, int doPrint) {
 // Reyva was driving
 if (n <= 1) {
   if (doPrint) {
     // the parent is a base case so print n
     fprintf(stdout, "%d\n", n);
     exit(0);
    }
    exit(n);

 } else {
    // Reyva stopped
    // Lucy was driving
    int status1;
    int status2;
    pid_t child1 = fork();
    // if pid equals 0 it is the child1
    if (child1 == 0) {
      doFib(n - 1, 0);
    } else if (child1 < 0) {
      // error incorrect pid value
      return;

    } else {
      pid_t child2 = fork();
      // if pid equals 0 then its child2
      if(child2 == 0){
        doFib(n - 2, 0);
      } else if (child2 < 0) {
        // error incorrect pid value
        return;
      }

      // parent thread
      int wait1 = waitpid(child1, &status1, 0);
      int wait2 = waitpid(child2, &status2, 0);
      if (wait1 == -1 || wait2 == -1) {
        return;
      }
      int result = WEXITSTATUS(status1) + WEXITSTATUS(status2);

      // if doPrint is true print otherwise provide the result to the parent
      if(doPrint){
        fprintf(stdout, "%d\n", result);
      } else {
        exit(result);
      }
      // Lucy stopped
    }
  }
}
