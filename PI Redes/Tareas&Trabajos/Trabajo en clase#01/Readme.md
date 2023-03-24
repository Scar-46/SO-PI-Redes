[Trabajo en clase: 2023/Mar/14; entrega 2023/Mar/17]

Objetivo
   Completar una clase "Socket" en C++ para lograr el intercambio de mensajes entre computadoras que no comparten memoria

Funcionamiento

   Vamos a trabajar con la comunicación entre varias máquinas, que no comparten memoria y que podemos
   denominar "Memoria Distribuida".  Siempre vamos a tener dos (o más) procesos que se comunican entre sí, pero
   ahora residen en máquinas distintas con arquitecturas que pueden diferir, que inclusive pueden tener sistemas
   operativos diferentes, pero que deben compartir un mismo protocolo de comunicación para que puedan entenderse.

   Uno de los esquemas para lograr esta comunicación es "Cliente-Servidor", en el que uno de los procesos en una 
   computadora lo denominaremos "cliente" y al otro, en otra computadora, "servidor".  Requerimos que ambas
   computadoras estén enlazadas por un canal de comunicación (red local).  Como indicamos antes, no es necesario
   que ambos programas corran bajo el mismo sistema operativo.

   En esta primera entrega, vamos a crear los métodos de una clase "Socket" en C++ para lograr que un proceso cliente
   se pueda conectar a un servidor.  En una segunda entrega, vamos a completar esta clase "Socket" con otros
   métodos para que tenga la funcionalidad requerida por un servidor.

   Paso de mensajes por medio de sistemas Linux, descripción lógica:

      ________________                                ________________
     |                |                              |                |
     |   -----------  |                              |   -----------  |
     |  |           | |                              |  |           | |
     |  | Proceso C | |                              |  | Proceso S | |
     |  |  Cliente  | |                              |  | Servidor  | |
     |   -----------  |                              |   -----------  |
     |       |        |                              |       |        |
     |  ============  |                              |  ============  |
     |  +          +  |         /~~~~~~~~~~\         |  +          +  |
     |  + Sist. Op.+  |         |  Red de  |         |  + Sist. Op.+  |
     |  +  (Linux) +  |<------->|  comunic.|<------->|  + (Linux)  +  |
     |  ============  |         \~~~~~~~~~~/         |  ============  |
     |   Máquina A    |                              |   Máquina B    |
     |                |                              |                |
     |________________|                              |________________|


   Para lograr la comunicación del proceso C con el proceso S, es necesario contar con la dirección del equipo
   donde reside el proceso S, o sea B, además requerimos la ubicación del proceso S.  En la jerga de redes de 
   comunicación, la ubicación del proceso S es denominado puerto.  Por lo tanto, para establecer la comunicación
   entre estos dos procesos, vamos a requerir ambos componentes: dirección de la máquina y dirección del proceso.
   Deben estudiar los llamados al sistema indicados para entender como desarrollar esta clase.

Tareas

   - Completar la clase C++ "Socket" para poder intercambiar mensajes entre procesos que no comparten memoria.
     Se facilita la interfaz "Socket.h".  Para la primera etapa deben completar:

        Socket, el constructor (socket)
        Close, para destruir el Socket (close)
        Connect, para conectarse (connect)
        Read, para leer información por el Socket (read)
        Write, para escribir por el Socket (write)

   - Instancias de la clase:
        int SocketId;
        int port;
        bool ipv6;

   - Los ejemplos "ipv4-test.cc" y "ipv6-test.cc" deben funcionar correctamente

   - Una vez construido el socket (socket), Unix lo interpreta como un archivo abierto por lo que es posible realizar
     lecturas (read) y escrituras (write) a ese "archivo"


Descripción detallada
   - Clase Socket

      Socket( char, bool = false );	// char = 's' for stream, 'd' for datagram, IPv6 = true if required

      ~Socket();			// call Close class method

      Close();				// close socket

      Connect( char * hostip, int port );		// connect to server (char *), and port (int)
         // Como utilizar la estructura de conexión, consultar el manual
         // Para IPv4
            struct sockaddr_in  host4;
            memset( (char *) &host4, 0, sizeof( host4 ) );
            host4.sin_family = AF_INET;
            inet_pton( AF_INET, hostip, &host4.sin_addr );
            host4.sin_port = htons( port );
            st = connect( idSocket, (sockaddr *) &host4, sizeof( host4 ) );
            if ( -1 == st ) {
               perror( "Socket::Connect" );
               exit( 2 );
            }

      Connect( char *, char *);		// connect to server (char *), and port (char *)
      Read( char *, int );		// reads from socket to buffer (char *), max (int bytes)
      Write( char *, int );		// writes to socket from buffer (char *), max (int bytes)

Referencias
   Recuerde que puede utilizar el manual (man) de los Unix para consultar los llamados al sistema indicados:
      socket
      close
      connect	(man connect)
      read	(man 3 read)
      write	(man 3 write)
      inet_pton
      htons

   https://os.ecci.ucr.ac.cr/ci0123/material/sockets-course.ppt