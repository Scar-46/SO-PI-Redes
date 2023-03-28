#include <stdio.h>
#include <unistd.h>
#include "Semaphore.h"


/**
  * 
 **/
int first() {
   printf( "Going first\n" );
}


/**
 * 
 */
int second() {
   printf( "Going second\n" );
}


/**
  *  This method calls explicitely first and second methods to display in this order
  *
 **/
int serialTest() {

   first();
   second();

}

int main() {
    Semaphore s( 0 );	// Need to start on zero

    serialTest();

    if ( fork() ) {
       // Need to wait until first completes (wait)
       s.Wait();

       second();
    }
    else {
       first();
       
       // Signal the other process to continue
       s.Signal();
    }

}
/*
   Salida esperada:
     Going first
     Going second
     Going first
     Going second
*/

