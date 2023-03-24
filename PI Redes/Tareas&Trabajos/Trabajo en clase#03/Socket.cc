/**
 *   CI0123 PIRO
 *   Clase para utilizar los "sockets" en Linux
 *
 **/

#include <unistd.h> 
#include <stdio.h>	// for perror
#include <stdlib.h>	// for exit
#include <string.h>	// for memset
#include <arpa/inet.h>	// for inet_pton
#include <sys/types.h>	// for connect 
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <netdb.h>


#include "Socket.h"

/**
  *  Class constructor
  *     use Unix socket system call
  *
  *  @param	char type: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param	bool ipv6: if we need a IPv6 socket
  *
 **/
Socket::Socket( char type, bool IPv6 ){
   int sType;
   if ( type == 's' ) {
      sType = SOCK_STREAM;
   } else {
      sType = SOCK_DGRAM;
   }
   if ( IPv6 ) {
      this->idSocket = socket( AF_INET6, sType, 0 );
   } else {
      this->idSocket = socket( AF_INET, sType, 0 );
   }
}


/**
  * Class destructor
  *
 **/
Socket::~Socket(){
   Close();
}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void Socket::Close(){
   if (this->idSocket != -1) {
      close(this->idSocket);
      this->idSocket = -1;
   }
}

/**
  * Connect method
  *   use "connect" Unix system call
  *
  * @param	char * host: host address in dot notation, example "10.1.104.187"
  * @param	int port: process address, example 80
  *
 **/
int Socket::Connect( const char * host, int port ) {
   int st;
   struct sockaddr_in  host4;
   struct sockaddr * ha;

   memset( (char *) &host4, 0, sizeof( host4 ) );
   host4.sin_family = AF_INET;
   inet_pton( AF_INET, host, &host4.sin_addr );
   host4.sin_port = htons( port );
   ha = (struct sockaddr *) &host4;
   st = connect( idSocket, (sockaddr *) ha, sizeof( host4 ) );
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Connect" );
      exit( 2 );
   }

   return st;
}


/**
  * Connect method
  *
  * @param	char * host: host address in dot notation, example "163.178.104.187"
  * @param	char * service: service name, example "http"
  *
 **/
int Socket::Connect( const char *host, const char *service ) {
   int st = -1;
   struct addrinfo hints, *result, *rp;
   int s;

   /* Obtain address(es) matching host/port */

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
   hints.ai_flags = 0;
   hints.ai_protocol = 0;          /* Any protocol */

   s = getaddrinfo(host, service, &hints, &result);
   if (s != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
      exit(2);
   }

   /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully connect(2).
      If socket(2) (or connect(2)) fails, we (close the socket
      and) try the next address. */

   for (rp = result; rp != NULL; rp = rp->ai_next) {
      idSocket = socket(rp->ai_family, rp->ai_socktype,
                        rp->ai_protocol);
      if (idSocket == -1)
         continue;

      if (connect(idSocket, rp->ai_addr, rp->ai_addrlen) != -1)
         break;                  /* Success */

      close(idSocket);
   }

   if (rp == NULL) {               /* No address succeeded */
      fprintf(stderr, "Could not connect\n");
      exit(2);
   }
   
   freeaddrinfo(result);           /* No longer needed */
   return st;
}


/**
  * Read method
  *   use "read" Unix system call (man 3 read)
  *
  * @param	void * text: buffer to store data read from socket
  * @param	int size: buffer capacity, read will stop if buffer is full
  *
 **/
int Socket::Read( void * text, size_t size ) {
   int st = -1;
   st = read(this->idSocket, text, size);
   if (st < 0) {
      perror("read error");
   }
   return st;
}


/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param	void * buffer: buffer to store data write to socket
  * @param	size_t size: buffer capacity, number of bytes to write
  *
 **/
int Socket::Write( const void *text, size_t size ) {
   int st = -1; 
   st = write(this->idSocket, text, size);
   if (st < 0) {
      perror("write error");
   }
   return st;
}


/**
  * Write method
  *
  * @param	char * text: string to store data write to socket
  *
  *  This method write a string to socket, use strlen to determine how many bytes
  *
 **/
int Socket::Write( const char *text ) {
   int st = -1;
   st = write(this->idSocket, text, strlen(text));
    if (st < 0) {
      perror("write error");
   }
   return st;
}


/**
  * Listen method
  *   use "listen" Unix system call (server mode)
  *
  * @param	int queue: max pending connections to enqueue 
  *
  *  This method define how many elements can wait in queue
  *
 **/
int Socket::Listen( int queue ) {
   int st = -1;

   return st;
}


/**
  * Bind method
  *    use "bind" Unix system call (man 3 bind) (server mode)
  *
  * @param	int port: bind a socket to a port defined in sockaddr structure
  *
  *  Links the calling process to a service at port
  *
 **/
int Socket::Bind( int port ) {
   int st;
   struct sockaddr_in6 server6;

   memset( (char *) &server6, 0, sizeof( server6 ) );
   server6.sin6_family = AF_INET6;
   server6.sin6_port = htons( port );
   server6.sin6_addr = in6addr_any;
   st = bind( idSocket, (sockaddr *) &server6, sizeof( server6 ) );
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Bind" );
      exit( 4 );
   }
   return st;
}


/**
  * Accept method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *  @returns	a new class instance
  *
  *  Waits for a new connection to service (TCP  mode: stream)
  *
 **/
Socket * Socket::Accept(){
   return (Socket *) 0;
}


/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown)
  *
  *  @param	int mode define how to cease socket operation
  *
  *  Partial close the connection (TCP mode)
  *
 **/
int Socket::Shutdown( int mode ) {
   int st = -1;
   return st;
}


/**
  *  SetIDSocket
  *
  *  @param	int id assigns a new value to field
  *  
 **/
void Socket::SetIDSocket(int id){
   this->idSocket = id;
}

int Socket::sendTo (void * buffer, int length, void *other ){
   return sendto(this->idSocket, buffer, length, 0, (struct sockaddr*) other, sizeof(sockaddr_in6));
}

int Socket::sendTo (const void * buffer, int length, void *other ){
   return sendto(this->idSocket, buffer, length, 0, (struct sockaddr*) other, sizeof(sockaddr_in6));
}

int Socket::recvFrom ( void * buffer, int length, void *other ){
   socklen_t len = sizeof(sockaddr_in6);
   return recvfrom(this->idSocket, buffer, length, 0, (struct sockaddr*) other, &len);
}


void Socket::InitSSLContext(){
   // We must create a method to define our context
   const SSL_METHOD * method = TLS_client_method();
   // Check for errors
   SSL_CTX * context = SSL_CTX_new( method );
   // Check for errors
   this->SSLContext = (void *) context;
   if ( NULL == this->SSLContext ) {
      perror("SSL_CTX");
      exit( 2 );
   }
}

void Socket::InitSSL() {
   this->InitSSLContext();
   SSL * ssl = SSL_new( (SSL_CTX *) this->SSLContext);  // this-> context
   // Check for errors
   this->SSLStruct = (void *) ssl;
   if ( NULL == ssl ) {
      perror("SSL_new");
      exit( 3 );
   }
}

int Socket::SSLConnect(char* host, int port){
   int st = -1;
   Connect(host, port);
   SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket);
   st = SSL_connect( (SSL *) this->SSLStruct);
   if (st < 0) {
      perror("SSL_connect error");
   }
   return st;
}

int Socket::SSLConnect(char* host, char* service){
   int st = -1;
   Connect(host, service);
   SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket);
   st = SSL_connect( (SSL *) this->SSLStruct);
   if (st < 0) {
      perror("SSL_connect error");
   }
   return st;
}

int Socket::SSLRead( void * buffer, size_t bufferSize ) {
   int st = -1;
   st = SSL_read((SSL *) this->SSLStruct, buffer, bufferSize);
   if (st < 0) {
      perror("SSL_read error");
   }
   return st;
}

int Socket::SSLWrite( void * buffer, size_t bufferSize ) {
   int st = -1;
   st = SSL_write((SSL *) this->SSLStruct, buffer, bufferSize);
   if (st < 0) {
      perror("SSL_write error");
   }
   return st;
}