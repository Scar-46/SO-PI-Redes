/**
  *  Ejemplo de sockets con SSL
  *
  *  Compilar con:
  *     g++ -g Socket.cc ipv4-ssl-cli.cc -lssl -lcrypto -o ssl-ipv4-ssl-cli.out
  *
 **/

#include <stdio.h>
#include <string.h>

#include "Socket.h"

#define SIZE 1024

int main( int argc, char * argv[] ) {
   char * os = (char *) "os.ecci.ucr.ac.cr";
   char * reqOS = (char *) "GET /lego/figure=octupus HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";
   Socket s( 's', false );	// Create a new stream socket for IPv4
   char a[ SIZE ];
   int count;

   memset( a, 0, SIZE );
   s.InitSSL();
   s.SSLConnect( os, (char *) "https" );
   s.SSLWrite(  (void *) reqOS, strlen( reqOS ) );
   while (  (count = s.SSLRead( a, SIZE )) > 0 ) {
      printf( "%s\n", a );
      memset( a, 0, SIZE );
   }

   printf( "%s\n", a );

}

