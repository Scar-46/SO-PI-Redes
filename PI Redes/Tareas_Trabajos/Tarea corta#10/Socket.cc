/**
 * @file Socket.cc
 * @copyright Copyright (c) 2023
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief socket class
 * @version 0.1
 * @date 2023-04-04
 * 
 */

#include <unistd.h>
#include <stdio.h>  // for perror
#include <stdlib.h>  // for exit
#include <string.h>  // for memset
#include <arpa/inet.h>  // for inet_pton
#include <sys/types.h>  // for connect
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <netdb.h>

#include "openssl/ssl.h"
#include "openssl/err.h"

#include "Socket.h"

Socket::Socket(char type, bool IPv6) {
  int sType;
  if (type == 's') {
    sType = SOCK_STREAM;
  } else {
    sType = SOCK_DGRAM;
  }
  if (IPv6) {
    this->idSocket = socket(AF_INET6, sType, 0);
    this->ipv6 = true;
  } else {
    this->idSocket = socket(AF_INET, sType, 0);
    this->ipv6 = false;
  }
}

Socket::Socket(int idSocket) {
  this->idSocket = idSocket;
}

Socket::~Socket() {
  if (this->SSLContext != NULL) {
    SSL_free(reinterpret_cast<SSL*>(this->SSLStruct));
    SSL_CTX_free(reinterpret_cast<SSL_CTX*>(this->SSLContext));
  }
  Close();
}

void Socket::Close() {
  if (this->idSocket != -1) {
    close(this->idSocket);
    this->idSocket = -1;
  }
}

int Socket::Connect( const char * host, int port ) {
  // inicializaciones
  int st;

  if (!ipv6){
    struct sockaddr_in  host4;
    struct sockaddr * ha;

    memset( (char *) &host4, 0, sizeof( host4 ) );
    host4.sin_family = AF_INET;
    host4.sin_port = htons( port );
    inet_pton( AF_INET, host, &host4.sin_addr );
    ha = (sockaddr *)&host4;
  
    st = connect( idSocket, (sockaddr *) ha, sizeof( host4 ) );

    if ( -1 == st ) {	// check for errors
      perror( "Socket::Connect" );
      exit( 2 );
    }
  } else {
    struct sockaddr_in6  host6;
    struct sockaddr * ha;

    memset( (char *) &host6, 0, sizeof( host6 ) );
    host6.sin6_family = AF_INET6;
    host6.sin6_port = htons(port);
    inet_pton(AF_INET6, host, &host6.sin6_addr );
    ha = (struct sockaddr* ) &host6;

    st = connect(this->idSocket, ha, sizeof(host6));

    if (-1 == st ) {	// check for errors
    perror( "Socket::Connect" );
    exit( 2 );
    }
  }
  return st;
}

int Socket::Connect(const char *host, const char *service) {
  int st = -1;
  struct addrinfo hints, *result, *rp;
  int s;

  /* Obtain address(es) matching host/port */

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;  /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;  /* Any protocol */

  s = getaddrinfo(host, service, &hints, &result);
  if (s != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(2);
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    this->idSocket = socket(rp->ai_family, rp->ai_socktype,
    rp->ai_protocol);
    if (this->idSocket == -1)
      continue;

    if (connect(this->idSocket, rp->ai_addr, rp->ai_addrlen) != -1)
      break;  /* Success */

    close(this->idSocket);
  }

  if (rp == NULL) {  /* No address succeeded */
    fprintf(stderr, "Could not connect\n");
    exit(2);
  }

  freeaddrinfo(result);  /* No longer needed */
  return st;
}

int Socket::Read(void * text, size_t size) {
  int st = -1;
  st = read(this->idSocket, text, size);
  if (st < 0) {
    perror("read error");
  }
  return st;
}

int Socket::Write(const void *text, size_t size) {
  int st = -1;
  st = write(this->idSocket, text, size);
  if (st < 0) {
    perror("write error");
  }
  return st;
}

int Socket::Write(const char *text) {
  int st = -1;
  st = write(this->idSocket, text, strlen(text));
  if (st < 0) {
    perror("write error");
  }
  return st;
}

int Socket :: Listen(int queue) {
  int st = listen(this->idSocket, queue);
  if (st < 0) {
    perror("listen error");
  }
  return st;
}

int Socket::Bind(int port) {
  int st;

  if (!ipv6){
    struct sockaddr_in server;

    memset(reinterpret_cast<char *>(&server), 0, sizeof(server));
    server.sin_family  = AF_INET;
    server.sin_port  = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    st = bind(this->idSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server));
    if (-1 == st) {  // check for errors
      perror("Socket::Bind");
      exit(4);
    }
  } else {
    struct sockaddr_in6 server;

    memset(reinterpret_cast<char *>(&server), 0, sizeof(server));
    server.sin6_family  = AF_INET6;
    server.sin6_port  = htons(port);
    server.sin6_addr = in6addr_any;
    st = bind(this->idSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server));
    if (-1 == st) {  // check for errors
      perror("Socket::Bind");
      exit(4);
    }
  }
  return st;
}

Socket * Socket :: Accept() {
  int st;
  struct sockaddr_in6 server;
  socklen_t len = sizeof(server);
  st = accept(this->idSocket, (struct sockaddr *) &server, &len);
  if (-1 == st) {  // check for errors
    perror("Socket::Accept");
    exit(0);
  }
  return new Socket(st);
}

int Socket :: Shutdown(int how) {
  int s = shutdown(idSocket, how);

  if (-1 == s) {
    perror("Socket::Shutdown");
    exit(0);
  }

  return s;
}

void Socket::SetIDSocket(int id) {
  this->idSocket = id;
}

int Socket::sendTo(void * buffer, int length, void *other) {
  return sendto(this->idSocket, buffer, length, 0, (struct sockaddr*) other,
    sizeof(sockaddr_in6));
}

int Socket::sendTo(const void * buffer, int length, void *other) {
  return sendto(this->idSocket, buffer, length, 0, (struct sockaddr*) other,
    sizeof(sockaddr_in6));
}

int Socket::recvFrom(void * buffer, int length, void *other) {
  socklen_t len = sizeof(sockaddr_in6);
  return recvfrom(this->idSocket, buffer, length, 0, (struct sockaddr*) other,
    &len);
}

void Socket::SSLInitContext() {
  // We must create a method to define our context
  const SSL_METHOD * method = TLS_client_method();
  // Check for errors
  if (NULL == method) {
    perror("SSL_CTX");
    exit(1);
  }

  SSL_CTX * context = SSL_CTX_new(method);
  // Check for errors
  if (NULL == context) {
    perror("SSL_CTX");
    exit(2);
  }

  this->SSLContext = reinterpret_cast<void *>(context);
}

void Socket::SSLInit() {
  this->SSLInitContext();

  SSL * ssl = SSL_new(reinterpret_cast<SSL_CTX *>(this->SSLContext));
  // Check for errors
  if (NULL == ssl) {
    perror("SSL_new");
    exit(3);
  }

  this->SSLStruct = reinterpret_cast<void *>(ssl);
}

int Socket::SSLConnect(const char* host, int port) {
  int st = -1;
  Connect(host, port);
  SSL_set_fd(reinterpret_cast<SSL *>(this->SSLStruct), this->idSocket);
  st = SSL_connect(reinterpret_cast<SSL *>(this->SSLStruct));
  if (st < 0) {
    perror("SSL_connect error");
  }
  return st;
}

int Socket::SSLConnect(const char* host, char* service) {
  int st = -1;
  Connect(host, service);
  SSL_set_fd(reinterpret_cast<SSL *>(this->SSLStruct), this->idSocket);
  st = SSL_connect(reinterpret_cast<SSL *>(this->SSLStruct));
  if (st < 0) {
    perror("SSL_connect error");
  }
  return st;
}

int Socket::SSLRead(void * buffer, size_t bufferSize) {
  int st = -1;
  st = SSL_read(reinterpret_cast<SSL *>(this->SSLStruct), buffer, bufferSize);
  if (st < 0) {
    perror("SSL_read error");
  }
  return st;
}

int Socket::SSLWrite(void * buffer, size_t bufferSize) {
  int st = -1;
  st = SSL_write(reinterpret_cast<SSL *>(this->SSLStruct), buffer, bufferSize);
  if (st < 0) {
    perror("SSL_write error");
  }
  return st;
}

void Socket::SSLoadCertificates(const char * certFileName, const char * keyFileName) {
  int st;
  SSL_CTX * context = reinterpret_cast<SSL_CTX *>(this->SSLContext);

  if (SSL_CTX_use_certificate_file(context, certFileName, SSL_FILETYPE_PEM ) <= 0) {	 // set the local certificate from CertFile
    st = SSL_get_error( (SSL *) this->SSLStruct, st );
    ERR_print_errors_fp(stderr);
    abort();
  }

  if ( SSL_CTX_use_PrivateKey_file(context, keyFileName, SSL_FILETYPE_PEM ) <= 0) {	// set the private key from KeyFile (may be the same as CertFile)
    st = SSL_get_error((SSL *) this->SSLStruct, st);
    ERR_print_errors_fp(stderr);
    abort();
  }

  if ( ! SSL_CTX_check_private_key(context)) {	// verify private key
    st = SSL_get_error((SSL *) this->SSLStruct, st);
    ERR_print_errors_fp(stderr);
    abort();
  }

}
void Socket::SSLInitServerContext() {
  SSL_library_init();
  OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
  SSL_load_error_strings();   /* load all error messages */

  SSL_METHOD const *method = TLS_server_method();
  if ( method == NULL ) {
    ERR_print_errors_fp( stderr );
    abort();
  }

  SSL_CTX *context = SSL_CTX_new( method );   /* create new context from method */
  if ( context == NULL ) {
    ERR_print_errors_fp( stderr );
    abort();
  }

  this->SSLContext = reinterpret_cast<void *>(context);
}

void Socket::SSLInitServer( const char * certFileName, const char * keyFileName) {
  int st;

  // Create a new context calling SSLInitServerContext
  this->SSLInitServerContext();
  // Create a SSL * variable using SSL_new()
  SSL * ssl = SSL_new(reinterpret_cast<SSL_CTX *>(this->SSLContext));
  // Check for errors
  if ( NULL == ssl ) {
    perror( "SSL_new" );
    exit( 3 );
  }
  // Assing SSL * variable to an instance variable
  this->SSLStruct = reinterpret_cast<void *>(ssl);
  // Load SSL certificates, using SSLLoadCertificates() method
  this->SSLoadCertificates(certFileName, keyFileName);
}

void Socket::SSLShowCerts() {
  X509 *cert;
  char *line;

  cert = SSL_get_peer_certificate( (SSL *) this->SSLStruct );		 // Get certificates (if available)
  if ( nullptr != cert ) {
    printf("Server certificates:\n");
    line = X509_NAME_oneline( X509_get_subject_name( cert ), 0, 0 );
    printf( "Subject: %s\n", line );
    free( line );
    line = X509_NAME_oneline( X509_get_issuer_name( cert ), 0, 0 );
    printf( "Issuer: %s\n", line );
    free( line );
    X509_free( cert );
  } else {
    printf( "No certificates.\n" );
  }

}

void Socket::SSLCreate( Socket * original) {
  int st;
  SSL * ssl; 
  // Constructs a new SSL * variable using SSL_new() function
  ssl = SSL_new(reinterpret_cast<SSL_CTX *>(original->SSLContext));
  // Check for errors
  if (nullptr == ssl) {
    perror("SSL_new");
    exit(3);
  }
  // Assign new variable to instance variable
  this->SSLStruct = reinterpret_cast<void *>(ssl);
  // change conection status  to SSL using SSL_set_fd() function
  st = SSL_set_fd(ssl, this->idSocket);
  //  -----------------  Servlet( ssl );         /* service connection */
  // Check for errors 
  if (st < 0) {
    perror("SSL_set_fd");
    exit(3);
  }
  
}

void Socket::SSLAccept(){
  int st = -1;
  // Call SSL_accept() to initiate TLS/SSL handshake
  st = SSL_accept(reinterpret_cast<SSL *>(this->SSLStruct));
  // Check for errors
  if (st < 0) {
    perror("SSL_accept error");
    exit(4);
  }
}

const char * Socket::SSLGetCipher() {
  const char * cipher;
  // Call SSL_get_cipher() and return the name
  cipher = SSL_get_cipher(reinterpret_cast<SSL *>(this->SSLStruct));
  return cipher;
}
