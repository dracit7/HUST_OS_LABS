
#ifndef _SEM_H
#define _SEM_H

#include <sys/sem.h>

union semun {
  int val; /* Value for SETVAL */
  struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array; /* Array for GETALL, SETALL */
  struct seminfo *__buf; /* Buffer for IPC_INFO */
};

int init_sem(int key, int size);
int free_sem(int semid);
void set_sem(int semid, int index, int n);
void P(int semid, int index);
void V(int semid, int index);

#endif