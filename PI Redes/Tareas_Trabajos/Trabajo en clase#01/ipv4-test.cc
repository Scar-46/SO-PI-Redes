/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket example
 */

#include <stdio.h>
#include <string.h>

#include "Socket.h"

int main( int argc, char * argv[] ) {
   char * os = (char *) "os.ecci.ucr.ac.cr";
   char * osn = (char *) "163.178.104.187";//"10.1.104.187";
   char * request = (char *) "GET /lego/list.php?figure=giraffe HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";

   Socket s( 's' );	// Create a new stream socket for IPv4
   char a[512];

   memset( a, 0, 512 );
   s.Connect( osn, 80 );
   s.Write(  request, strlen( request ) );
   s.Read( a, 512 );
   printf( "%s\n", a);

}
