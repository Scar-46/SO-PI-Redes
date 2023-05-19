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
  } else {
    this->idSocket = socket(AF_INET, sType, 0);
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

int Socket::Connect(const char * host, int port) {
  int st;
  struct sockaddr_in  host4;
  struct sockaddr * ha;

  memset(reinterpret_cast<char *>(&host4), 0, sizeof( host4 ));
  host4.sin_family = AF_INET;
  inet_pton(AF_INET, host, &host4.sin_addr);
  host4.sin_port = htons(port);
  ha = (struct sockaddr *) &host4;
  st = connect(this->idSocket, reinterpret_cast<sockaddr *>(ha), sizeof(host4));
  if ( -1 == st ) {  // check for errors
    perror("Socket::Connect");
    exit(2);
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
  struct sockaddr_in server;

  memset(reinterpret_cast<char *>(&server), 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  st = bind(this->idSocket, reinterpret_cast<sockaddr *>(&server), sizeof(server));
  if (-1 == st) {  // check for errors
    perror("Socket::Bind");
    exit(4);
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

void Socket::InitSSLContext() {
  // We must create a method to define our context
  const SSL_METHOD * method = TLS_client_method();
  // Check for errors
  SSL_CTX * context = SSL_CTX_new(method);
  // Check for errors
  this->SSLContext = reinterpret_cast<void *>(context);
  if (NULL == this->SSLContext) {
    perror("SSL_CTX");
    exit(2);
  }
}

void Socket::InitSSL() {
  this->InitSSLContext();
  // this-> context
  SSL * ssl = SSL_new(reinterpret_cast<SSL_CTX *>(this->SSLContext));
  // Check for errors
  this->SSLStruct = reinterpret_cast<void *>(ssl);
  if (NULL == ssl) {
    perror("SSL_new");
    exit(3);
  }
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
