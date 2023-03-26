#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef RCV

#include "MailBox.h"
#define MAXDATA 1024

int main( int argc, char ** argv ) {

   int st;
   MailBox m;
   char data[ MAXDATA ];

   st = m.receive( 2023, (void *) data, sizeof( data ) );  // Receives a message with 2023 type
   while ( st > 0 ) {
      printf("Label: %s\n", data );
      st = m.receive( 2023, (void *) data, sizeof( data ) );
   }

}

#endif // RCV
