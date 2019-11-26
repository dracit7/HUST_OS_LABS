#include <stdio.h>
#include <pthread.h>

#include "lib/log.h"
#include "lib/sem.h"

#define SENUM_CALC 0
#define SENUM_PRINT 1

// The variable to be shared by two threads(`a`).
int shared_var = 0;

// The id of sem
int semid = 0;

// Thread 1's work
void* task_calculate(void* args) {

  // Add from 1 to 100 to `a`
  for (int i = 1; i <= 100; i++) {
    P(semid, SENUM_CALC); /* wait until the printer finishs its work */
    shared_var += i; /* update the value */
    V(semid, SENUM_PRINT); /* tell printer it's safe to print the new value */
  }
}

// Thread 2's work
void* task_print(void* args) {

  for (int i = 1; i <= 100; i++) {
    P(semid, SENUM_PRINT); /* wait until the calculator finishs its work */
    printf("%d\n", shared_var); /* print the value */
    V(semid, SENUM_CALC); /* tell calculator it's safe to update the value */
  }
}

int main(int argc, char const *argv[]) {

  int err;
  pthread_t threads[2];

  // Get the sem.
  semid = init_sem(IPC_PRIVATE, 2);
  if (semid < 0) {
    fault("failed to init sem.\nError code: %d\n", semid);
  }

  // Set the initial value of the sem.
  set_sem(semid, SENUM_CALC, 1);
  set_sem(semid, SENUM_PRINT, 0);

  // Create threads.
  err = pthread_create(&threads[0], NULL, task_calculate, NULL);
  if (err != 0) {
    fault("failed to create thread CALC.\nError code: %d\n", err);
  }
  
  err = pthread_create(&threads[1], NULL, task_print, NULL);
  if (err != 0) {
    fault("failed to create thread PRINT.\nError code: %d\n", err);
  }

  // Wait for the task to finish.
  pthread_join(threads[1], NULL);

  // Free the sem.
  err = free_sem(semid);
  if (err != 0) {
    fault("failed to free sem.\nError code: %d\n", err);
  }

  return 0;
}
