
#include "log.h"
#include "sem.h"

// Create a sem
int init_sem(int key, int size) {
	return semget(key, size, IPC_CREAT | 0666);
}

// Delete a sem
int free_sem(int semid) {
	return semctl(semid, 0, IPC_RMID, 0);
}

// Set the value of a sem
void set_sem(int semid, int index, int n) {

	union semun semopts; 
	semopts.val = n; // set the value of sem as `n`

	semctl(semid, index, SETVAL, semopts);
}

void P(int semid, int index) {

	struct sembuf sem;

	sem.sem_num = index;
	sem.sem_op = -1;
	sem.sem_flg = 0;

	semop(semid, &sem, 1);
}

void V(int semid, int index) {

	struct sembuf sem;

	sem.sem_num = index;
	sem.sem_op = 1;
	sem.sem_flg = 0;

  semop(semid, &sem, 1);
}
