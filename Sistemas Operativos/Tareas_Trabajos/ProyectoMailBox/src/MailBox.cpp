/**
 * @file MailBox.cpp
 * @author Operating systems (Francisco Arroyo)
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief C++ class to encapsulate Unix shared memory intrinsic structures and system calls
 * @version 0.1
 * @date 2023-04-11
 * Ref.: https://en.wikipedia.org/wiki/Shared_memory
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>

#include "MailBox.h"

#define KEY 0xC12840
#define MAXDATA 512 // maximum size of message data


MailBox::MailBox() {
  int st;

  st = msgget( KEY, IPC_CREAT | 0600 );
  if ( -1 == st ) {
    perror( "MailBox error: MailBox" );
    exit( 1 );
  }

  this->id = st;
}

MailBox::~MailBox() {
  int st = -1;
  // check for errors
  st = msgctl( this->id, IPC_RMID, NULL );
  if ( -1 == st ) {
    perror( "MailBox error: ~MailBox" );
    exit( 1 );
  }
}

int MailBox::send(long type, void* buffer, int numBytes) {
  int st = -1;

  // must declare a msgbuffer variable and set all the fields
  struct message m;
  m.mtype = type;
  memcpy((void*)m.mnums, buffer, numBytes);

  // use msgsnd system call to send message to a queue
  st = msgsnd(this->id, (void *) &m, numBytes, 0);
  if (st == -1) {
    perror("MailBox error: send");
    exit(1);
  }
  return st;
}

int MailBox::receive( long type, void * buffer, int capacity ) {
  int st = -1;

  // must declare a msgbuffer variable 
  struct message m;

  // use msgrcv system call to receive a message from the queue
  st = msgrcv( this->id, &m, capacity, type, 0);
  if (st == -1) {
    perror("MailBox error: receive");
    exit(1);
  }
  memcpy(buffer, (void*)m.mnums, capacity);

  return st;
}
