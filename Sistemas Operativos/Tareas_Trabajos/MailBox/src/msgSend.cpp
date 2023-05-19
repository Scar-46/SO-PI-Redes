#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef SND

#include "MailBox.h"

const char * labels[] = {
   "a",
   "b",
   "c",
   "d",
   "e",
   "li",
   ""
};

int main( int argc, char ** argv ) {


   int i ;
   MailBox m;
   
   i = 0;

   while ( strlen( labels[ i ] ) ) {
      m.send( 2023, (void *) labels[ i ], sizeof( labels[ i ] ) );  // Send a message with 2023 type
      printf("Label: %s\n", labels[ i ] );
      i++;
   }
   _exit(2);
}

#endif // SND
