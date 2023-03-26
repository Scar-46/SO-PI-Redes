[Ejercicio propuesto: 2023/Mar/14; entrega 2023/Mar/17]

Objetivo
   Completar una clase "Socket" en C++ para lograr el intercambio de mensajes entre computadoras que no comparten memoria
   utilizando el protocolo TCP en IPv6

Tareas

   - Completar la clase C++ "Socket" para poder intercambiar mensajes entre procesos que no comparten memoria.
     Se facilita la interfaz "Socket.h".  Para la esta etapa deben completar la funcionalidad para IPv6:
        Socket, el constructor (socket en IPv6)
        Connect, para conectarse (connect)

   - Los ejemplos "ipv4-test.cc" y "ipv6-test.cc" deben funcionar correctamente

Descripción detallada
   - Clase Socket
      Connect( char * hostip, int port );		// connect to server (char *), and port (int)
         // Para IPv6
            struct sockaddr_in6  host6;
            struct sockaddr * ha;
            host6.sin6_family = AF_INET6;
            st = inet_pton( AF_INET6, hostip, &host6.sin6_addr );
            if ( 1 == st ) {	// 0 means invalid address, -1 means address error
               perror( "Socket::Connect, inet_pton" );
               exit( 2 );
            }
            host6.sin6_port = htons( port );
            ha = (struct sockaddr *) &host6;
            len = sizeof( host6 );

      Connect( char *, char *);		// connect to server (char *), and port (char *)

Referencias
   https://os.ecci.ucr.ac.cr/ci0123/material/sockets-course.ppt
