/**
 * @file Socket.h
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @copyright Copyright (c) 2023
 * @brief Establece la definición de la clase Socket para efectuar la
 * comunicación de procesos que no comparten memoria, utilizando un esquema de
 * memoria distribuida.  El desarrollo de esta clase se hará en varias etapas,
 * primero los métodos necesarios para los clientes, en la otras etapas los
 * métodos para el servidor, manejo de IPv6, conexiones seguras y otros
 * 
 * Universidad de Costa Rica
 * ECCI
 * CI0123 Proyecto integrador de redes y sistemas operativos
 * 2023-i
 * Grupos: 2 y 3
 *
 * (versión Ubuntu)
 * @version 0.1
 * @date 2023-04-13
 */

#ifndef Socket_h
#define Socket_h

class Socket {
 public:
  // char == 's' => stream, char == 'd' => datagram
  /**
   * @brief Read bytes from a TLS/SSL connection using the SSL_read procedure.
   * 
   * @param buffer 
   * @param bufferSize 
   * @return int -1 on failure.
   */
  explicit Socket(char, bool IPv6 = false);

  /**
   * @brief Construct a new Socket object
   * 
   * @param int socket descriptor
   */
  explicit Socket(int);

  /**
   * @brief Destroy the  Socket object
   * 
   */
  ~Socket();

  /**
   * Connect method
   *  use "connect" Unix system call
   *
   * @param	char * host: host address in dot notation, example "10.1.104.187"
   * @param	int port: process address, example 80
   *
   **/
  int Connect(const char * host, int port);

  /**
   * Connect method
   *
   * @param	char * host: host address in dot notation, example "163.178.104.187"
   * @param	char * service: service name, example "http"
   *
   **/
  int Connect(const char * host, const char * service);

  /**
   * Close method
   * use Unix close system call (once opened a socket is managed like a
   * file in Unix)
   *
   **/
  void Close();

  /**
   * Read method
   *  use "read" Unix system call (man 3 read)
   *
   * @param	void * text: buffer to store data read from socket
   * @param	int size: buffer capacity, read will stop if buffer is full
   *
   **/
  int Read(void * buffer, size_t bufferSize);

  /**
   * Write method
   *  use "write" Unix system call (man 3 write)
   *
   * @param	void * buffer: buffer to store data write to socket
   * @param	size_t size: buffer capacity, number of bytes to write
   *
   **/
  int Write(const void * buffer, size_t bufferSize);

  /**
   * Write method
   *
   * @param	char * text: string to store data write to socket
   *
   * This method write a string to socket, use strlen to determine how many bytes
   *
   **/
  int Write(const char * buffer);  // size is calculated with strlen

  /**
   * Listen method
   *  use "listen" Unix system call (server mode)
   *
   * @param	int queue: max pending connections to enqueue 
   *
   *  This method define how many elements can wait in queue
   *
   **/
  int Listen(int backlog);

  /**
   * Bind method
   *  use "bind" Unix system call (man 3 bind) (server mode)
   *
   * @param	int port: bind a socket to a port defined in sockaddr structure
   *
   *  Links the calling process to a service at port
   *
   **/
  int Bind(int port);

  /**
   * @brief Accepts a new connection on a socket using the accept procedure.
   * 
   * @return Socket*  A new socket descriptor for the accepted connection.
   */
  Socket * Accept();

  /**
   * Shutdown method
   *  use "shutdown" Unix system call (man 3 shutdown)
   *
   *  @param	int mode define how to cease socket operation
   *
   *  Partial close the connection (TCP mode)
   *
   **/
  int Shutdown(int mode);  // mode = { SHUT_RD, SHUT_WR, SHUT_RDWR }

  /**
   *  SetIDSocket
   *
   *  @param	int id assigns a new value to field
   *  
   **/
  void SetIDSocket(int newId);

  /**
   * @brief Sends a message on socket using the sendto procedure.
   * 
   * @param buffer The message to be send.
   * @param length The length of the message.
   * @param other The destination address.
   * @return int 
   */
  int sendTo(void * buffer, int length, void *other);

  /**
   * @brief Sends a message on socket using the sendto procedure.
   * 
   * @param buffer The message to be send.
   * @param length The length of the message.
   * @param other The destination address.
   * @return int -1 on error.
   */
  int sendTo(const void * buffer, int length, void *other);

  /**
   * @brief Receives a message from a socket using the recvfrom procedure.
   * 
   * @param buffer The destination buffer.
   * @param length The length of the message.
   * @param other The address of the source.
   * @return int -1 on error.
   */
  int recvFrom(void * buffer, int length, void *other);

  // Métodos para conexiones seguras

  /**
   * @brief Begins the process to start a new SSL Context.
   * 
   */
  void SSLInit();

  /**
   * @brief Creates and define a new SSL context.
   * 
   */
  void SSLInitContext();

  /**
   * @brief Initiate the TLS/SSL handshake with an TLS/SSL server for IPv4 using
   * SSL_connect.
   * 
   * @param host 
   * @param port 
   * @return int -1 of failure.
   */
  int SSLConnect(const char * host, int port);

  /**
   * @brief Initiate the TLS/SSL handshake with an TLS/SSL server for IPv6 using
   * SSL_connect.
   * 
   * @param host 
   * @param service 
   * @return int -1 on failure
   */
  int SSLConnect(const char * host, char * service);

  /**
   * @brief Read bytes from a TLS/SSL connection using the SSL_read procedure.
   * 
   * @param buffer 
   * @param bufferSize 
   * @return int -1 on failure.
   */
  int SSLRead(void * buffer, size_t bufferSize);

  /**
   * @brief Write bytes from a TLS/SSL connection using the SSL_write procedure.
   * 
   * @param buffer 
   * @param bufferSize 
   * @return int -1 on failure.
   */
  int SSLWrite(void * buffer, size_t bufferSize);

  /**
   * @brief Load the certificate and private key from a file.
   * 
   * @param certFileName 
   * @param keyFileName 
   * @return int -1 on failure.
   */
  void SSLoadCertificates( const char * certFileName, const char * keyFileName );

  /**
   * @brief show the certificate information.
   * 
   */
  void SSLShowCerts();

  /**
   * @brief Initialize the server context.
   */
  void SSLInitServerContext();

  /**
   * @brief Initiate the SSL server.
   * 
   * @param certFileName certificate file name.
   * @param keyFileName private key file name.
   */
  void SSLInitServer(const char * certFileName, const char * keyFileName);

  /**
   * @brief create a new SSL connection.
   * 
   */
  void SSLCreate(Socket * original);

  /**
   * @brief Initiate the TLS/SSL handshake with an TLS/SSL server using SSL_accept.
   * 
   */
  void SSLAccept();

  /**
   * @brief Get the cipher name.
   * 
   * @return const char* 
   */
  const char * SSLGetCipher();

 private:
  /// @brief The socket descriptor.
  int idSocket;
  /// @brief The port number.
  int port;
  /// @brief The host address.
  bool ipv6;
  /// @brief SSL context for secure connections.
  void* SSLContext;  // SSL context
  /// @brief SSL structure.
  void* SSLStruct;  // SSL BIO basis input output
};

#endif
