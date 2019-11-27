#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>

#include "lib/log.h"
#include "lib/sem.h"
#include "lib/buffer.h"

#define SEM_SPACE_LEFT 0
#define SEM_SPACE_USED 1
#define SEM_MUTEX 2

#define RING_BUF_SIZE 16

#define PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH)

// Global variables
int semid; // Semaphore
int shmid; // Shared memory

int main(int argc, char const *argv[]) {

  int fd_read, fd_write;
  int pid_read, pid_write;
  int err;
  ring_buf *buf, *shared_buf_1, *shared_buf_2; // Ring buffer

  // Check arguments' sanity
  if (argc != 3) {
    printf("Invalid arguments.\nUsage: lab3 <filea> <fileb>\n");
    exit(1);
  }

  // Open files
  fd_read = open(argv[1], O_RDONLY);
  fd_write = open(argv[2], O_RDWR | O_CREAT, PERM);
  if (fd_read < 0 || fd_write < 0) {
    fault("can not open files.\nfd_read: %d\nfd_write: %d\n", fd_read, fd_write);
  }

  // Get the sem
  semid = init_sem(IPC_PRIVATE, 3);
  if (semid < 0) {
    fault("failed to init sem.\nError code: %d\n", semid);
  }

  // Set the initial value of the sem.
  set_sem(semid, SEM_SPACE_LEFT, RING_BUF_SIZE);
  set_sem(semid, SEM_SPACE_USED, 0);
  set_sem(semid, SEM_MUTEX, 1); 

  // Get the shared memory
  shmid = shmget(
    IPC_PRIVATE, 
    sizeof(ring_buf) + RING_BUF_SIZE*sizeof(ring_buf_node), 
    IPC_CREAT | PERM
  );
  if (shmid < 0) {
    fault("failed to allocate a piece of shared memory.\nError code: %d\n", shmid);
  }

  // Attach the shared memory to the parent process,
  // and place the buffer into this piece of memory.
  buf = shmat(shmid, NULL, 0);
  memset(buf, 0, sizeof(ring_buf) + RING_BUF_SIZE*sizeof(ring_buf_node));
  buf->nodes = (ring_buf_node *)(buf + sizeof(ring_buf));
  buf->size = RING_BUF_SIZE;

  // Fork out the writer process
  switch(pid_write = fork()) {
  case -1: /* error */
    fault("fork() failed.\nError code %d\n", err);

  case 0: /* Child process */

    // Access the buffer by shared memory
    shared_buf_1 = (ring_buf*)shmat(shmid, NULL, 0);
    if (shared_buf_1 == (void *)-1) {
      fault("failed to share memory in child process.", 0);
    }

    // Write the file into buffer
    while (1) {

      P(semid, SEM_SPACE_LEFT); // Mark that the left space in buffer decreased by 1.
      P(semid, SEM_MUTEX); // Lock the buffer to avoid race condition

      // Write to buf
      switch(write_to_buf(fd_read, shared_buf_1)) {
        // Error cases
        case -ERR_FULL: /* this should not happen since we uses semaphore */
          fault("the buffer is full.", 0);
        case -ERR_READ: /* general error */
          fault("read error.", 0);

        // General case
        case 0: /* no error */
          V(semid, SEM_MUTEX); // Release the lock
          V(semid, SEM_SPACE_USED); // Mark that one block has been written to the buffer.
          continue;

        // The file is over
        case -ERR_END:
          close(fd_read);
          /* Only this case leaves the switch */
      }

      V(semid, SEM_MUTEX);
      V(semid, SEM_SPACE_USED);
      break;
    }

    // The child process exits after finishing its work.
    exit(0);
  }

  // Fork out the reader process
  switch(pid_read = fork()) {
  case -1: /* error */
    fault("fork() failed.\nError code %d\n", err);

  case 0: /* Child process */

    // Access the buffer by shared memory
    shared_buf_2 = (ring_buf*)shmat(shmid, NULL, 0);
    if (shared_buf_2 == (void *)-1) {
      fault("failed to share memory in child process.", 0);
    }

    // Write the content in buffer into file.
    while (1) {

      P(semid, SEM_SPACE_USED); // Mark that one block in buffer has been read.
      P(semid, SEM_MUTEX); // Lock the buffer to avoid race condition

      // Read from buf
      switch(read_from_buf(fd_write, shared_buf_2)) {
        // Error cases
        case -ERR_EMPTY: /* this should not happen since we uses semaphore */
          fault("the buffer is empty.\n", 0);
        case -ERR_WRITE: /* general error */
          fault("write error.", 0);

        // General case
        case 0: /* no error */
          V(semid, SEM_MUTEX); // Release the lock
          V(semid, SEM_SPACE_LEFT); // Mark that the left space in buffer increased by 1.
          continue;

        // The file is over
        case -ERR_END:
          close(fd_write);
          /* Only this case leaves the switch */
      }

      V(semid, SEM_MUTEX);
      V(semid, SEM_SPACE_LEFT);
      break;
    }

    // The child process exits after finishing its work.
    exit(0);
  }

  // Wait for the worker processes
  waitpid(pid_read, &err, 0);
  waitpid(pid_write, &err, 0);

  free_sem(semid);

  return 0;
}
