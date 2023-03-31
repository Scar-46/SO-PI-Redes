/**
  *  C++ class to encapsulate Unix shared memory intrinsic structures and system calls
  *  Author: Operating systems (Francisco Arroyo)
  *  Version: 2023/Mar/15
  *
  * Ref.: https://en.wikipedia.org/wiki/Shared_memory
  *
 **/

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


struct message {
   long mtype;
   int mnums[2];
};


/**
  *  Class constructor
  *
  *  Must call "msgget" to create a mailbox
  *
  *  MailBoxkey is your student id number: 0xA12345 (to represent as hexadecimal value)
  *  size = 1
  *  MailBoxflg: IPC_CREAT | 0600
  *
 **/
MailBox::MailBox() {
   int st;

   st = msgget( KEY, IPC_CREAT | 0600 );
   if ( -1 == st ) {
      perror( "MailBox error: MailBox" );
      exit( 1 );
   }

   this->id = st;

}


/**
  *   Class destructor
  *
  *   Must call msgctl
  *
 **/
MailBox::~MailBox() {
   int st = -1;

   // call msgctl to destroy this message queue
   // check for errors
   st = msgctl( this->id, IPC_RMID, NULL );
   if ( -1 == st ) {
      perror( "MailBox error: ~MailBox" );
      exit( 1 );
   }
}

/**
  *   Send method
  *
  *   Need to call msgsnd to receive a pointer to shared memory area
  *
  *   other fields must come as parameters, or build a specialized struct
  *
 **/
int MailBox::send(long type, void* buffer, int numBytes) {
   int st = -1;

   // must declare a msgbuffer variable and set all the fields
   struct message m;
   m.mtype = type;
   memcpy((void*)m.mnums, buffer, numBytes);
   // set other fields if necessary

   // use msgsnd system call to send message to a queue
   st = msgsnd(this->id, (void *) &m, numBytes, 0);
   if (st == -1) {
      perror("MailBox error: send");
      exit(1);
   }
   return st;

}


/**
  *   receive method
  *
  *   Need to call msgrecv to receive messages from queue
  *
  *   Remember rules to receive message, see documentation
  *
 **/
int MailBox::receive( long type, void * buffer, int capacity ) {
   int st = -1;

   // must declare a msgbuffer variable 
   struct message m;

   // use msgrcv system call to receive a message from the queue
   //  st = msgrcv( this->id, (void*) &m, sizeof(m.data), type, 0);
   st = msgrcv( this->id, &m, capacity, type, 0);
   if (st == -1) {
      perror("MailBox error: receive");
      exit(1);
   }
   memcpy(buffer, (void*)m.mnums, capacity);

   return st;

}
