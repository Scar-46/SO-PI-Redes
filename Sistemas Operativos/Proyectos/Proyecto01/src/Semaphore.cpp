/**
  *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
  *  Author: Operating systems (Francisco Arroyo)
  *  co-author: Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
  *  Version: 2023/Mar/15
  * @copyright Copyright (c) 2023
  * Ref.: https://en.wikipedia.org/wiki/Semaphore_(programming)
  *
 **/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "Semaphore.h"

/**
 *   Union definition to set an initial value to semaphore
 *   Calling program must define this structure
 *   @struct semun
 */
union semun {
  /// @brief  Value for SETVAL
  int val;
  /// @brief Buffer for IPC_STAT, IPC_SET
  struct semid_ds *buf;
  /// @brief Array for GETALL, SETALL
  unsigned short *array;
  /// @brief Buffer for IPC_INFO (Linux-specific)
  struct seminfo *__buf;
};

Semaphore::Semaphore(int key, int initialValue) {
  union semun x;
  id = semget(key, 1, IPC_CREAT | 0600);
  // Call semget to construct semaphore array
  x.val = initialValue;
  // Call semctl to set initial value
  semctl(id, 0, SETVAL, x);
}

Semaphore::~Semaphore() {
  semctl(id, 0, IPC_RMID);
}


int Semaphore::Signal() {
  int st = -1;
  struct sembuf z;

  z.sem_num = 0;
  z.sem_op  = 1;
  z.sem_flg = 0;

  // call semop
  semop(id, &z, 1);

  return st;
}

int Semaphore::Wait() {
  int st = -1;
  struct sembuf z;

  z.sem_num = 0;
  z.sem_op  = -1;
  z.sem_flg = 0;

  // call semop
  semop(id, &z, 1);

  return st;
}
