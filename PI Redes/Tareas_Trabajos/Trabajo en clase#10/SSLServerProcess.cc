/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket client/server example with process
 *
 **/

#include <cstdlib>	// atoi
#include <cstdio>	// printf
#include <cstring>	// strlen, strcmp
#include <unistd.h>

#include "Socket.h"

#define PORT	4321


void Service( Socket * client ) {
   char buf[ 1024 ] = { 0 };
   int sd, bytes;
   const char* ServerResponse="\n<Body>\n\
\t<Server>os.ecci.ucr.ac.cr</Server>\n\
\t<dir>ci0123</dir>\n\
\t<Name>Proyecto Integrador Redes y sistemas Operativos</Name>\n\
\t<NickName>PIRO</NickName>\n\
\t<Description>Consolidar e integrar los conocimientos de redes y sistemas operativos</Description>\n\
\t<Author>profesores PIRO</Author>\n\
</Body>\n";
    const char *validMessage = "\n<Body>\n\
\t<UserName>piro</UserName>\n\
\t<Password>ci0123</Password>\n\
</Body>\n";

   client->SSLAccept();
   client->SSLShowCerts();

   bytes = client->SSLRead( buf, sizeof( buf ) );
   buf[ bytes ] = '\0';
   printf( "Client msg: \"%s\"\n", buf );

   if ( ! strcmp( validMessage, buf ) ) {
      client->SSLWrite( (void*) ServerResponse, strlen( ServerResponse ) );
   } else {
      client->SSLWrite( (void*) "Invalid Message", strlen( "Invalid Message" ) );
   }

   client->Close();

}


int main( int cuantos, char ** argumentos ) {
   Socket * server, * client;
   int port = PORT;
   
   if ( cuantos > 1 ) {
      port = atoi( argumentos[ 1 ] );
   }

   server = new Socket( 's' );
   server->Bind( port );
   server->Listen( 10 );
   server->SSLInitServer( "server-cert.pem" , "server-key.pem");

    int childpid;

   for( ; ; ) {
      client = server->Accept();
      client->SSLCreate( server );
      childpid = fork();
      if (childpid < 0) {
        perror("ERROR on fork");
        exit(1);
      }
      if(0 == childpid) {
        server->Close();
        Service(client);
        exit(0);
      }
      client->Close();
   }

}
