#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "semaphore.h"

Semaphore::Semaphore(int count)
{
  semid = semget(IPC_PRIVATE, 1, 0600);
  if (semid == -1)
  {
    perror("semget");
    exit(1);
  }
  semctl(semid, 0, SETVAL, count);
}

Semaphore::~Semaphore()
{
  semctl(semid, 0, IPC_RMID);
}

void Semaphore::Wait()
{
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = 0;
  semop(semid, &sb, 1);
}

void Semaphore::Signal()
{
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = 1;
  sb.sem_flg = 0;
  semop(semid, &sb, 1);
}