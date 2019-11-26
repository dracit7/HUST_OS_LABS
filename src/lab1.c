#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "lib/log.h"

// Global variables
int childs[2]; /* Child processes' pids */
int pipe_fds[2]; /* file descriptors of each side of pipe */

// If user want to kill parent, kill child processes and quit.
void parent_sigkill_handler(int code) {

  int err;
  
  // Kill two child processes separately
  err = kill(childs[0], SIGUSR1);
  if (err < 0) {
    fault("failed to kill child 1.\nError code %d\n", err);
  }
  err = kill(childs[1], SIGUSR1);
  if (err < 0) {
    fault("failed to kill child 2.\nError code %d\n", err);
  }

}

void child1_sigkill_handler(int code) {
  printf("Child process 1 is killed by parent!\n");
  close(pipe_fds[1]);
  exit(0);
}

void child2_sigkill_handler(int code) {
  printf("Child process 2 is killed by parent!\n");
  close(pipe_fds[0]);
  exit(0);
}

int main(int argc, char const *argv[]) {

  // Variables
  int err;

  // Create the pipe for two child processes
  err = pipe(pipe_fds);
  if (err < 0) {
    fault("can not create a new pipe.\nError code %d\n", err);
  }

  // Fork out a child process
  switch(childs[0] = fork()) {
  case -1: /* error */
    fault("fork() failed.\nError code %d\n", err);
  
  case 0: /* Child process */
    close(pipe_fds[0]); /* Read endpoint is not used */

    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1, child1_sigkill_handler); /* Set the signal handler */

    int x = 1;
    while (1) { /* Send messages to the other child process periodically */
      sleep(1);
      write(pipe_fds[1], (void *)&x, sizeof(x));
      x++;
    }
  
  }

  /* Parent process */

  // Fork out another child process
  switch(childs[1] = fork()) {
  case -1: /* error */
    fault("fork() failed.\nError code %d\n", err);
  
  case 0: /* Child process */
    close(pipe_fds[1]); /* Write endpoint is not used */

    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1, child2_sigkill_handler); /* Set the signal handler */

    int x;
    while (1) { /* Send messages to the other child process periodically */
      read(pipe_fds[0], (void *)&x, sizeof(x));
      printf("I send you %d times.\n", x);
    }
  }

  signal(SIGINT, parent_sigkill_handler);

  // Wait for the child processes to quit
  waitpid(childs[0], &err, 0);
  waitpid(childs[1], &err, 0);

  // Close the pipe and exit
  close(pipe_fds[0]);
  close(pipe_fds[1]);

  printf("Parent Process is killed!\n");
  return 0;
}
