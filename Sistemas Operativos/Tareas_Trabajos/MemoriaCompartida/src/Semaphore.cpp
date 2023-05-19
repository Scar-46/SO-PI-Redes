/**
  *  C++ class to encapsulate Unix semaphore intrinsic structures and system calls
  *  Author: Operating systems (Francisco Arroyo)
  *  Version: 2023/Mar/15
  *
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
  *
 **/
union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};


/**
  *  Class constructor
  *
  *  Must call "semget" to create a semaphore array and "semctl" to define a initial value
  *
  *  semkey is your student id number: 0xA12345 (to represent as hexadecimal value)
  *  nsems = 1
  *  semflg: IPC_CREAT | 0600
  *
 **/
Semaphore::Semaphore( int initialValue ) {
   union semun x;
   id = semget(0xC12840, 1, IPC_CREAT | 0600);
   // Call semget to construct semaphore array

   x.val = initialValue;
   // Call semctl to set initial value
   semctl(id, 0, SETVAL, x);
}


/**
  *   Class destructor
  *
  *   Must call semctl
  *
 **/
Semaphore::~Semaphore() {
   semctl(id, 0, IPC_RMID);
}


/**
  *   Signal method
  *
  *   Need to call semop to add one to semaphore
  *
 **/
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


/**
  *   Wait method
  *
  *   Need to call semop to substract one to semaphore
  *
 **/
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

