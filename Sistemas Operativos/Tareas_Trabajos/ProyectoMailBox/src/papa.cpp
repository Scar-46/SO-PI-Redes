/**
 *  CI0122 Sistemas Operativos
 *  Grupo 2, 2023-i
 *
 *  Problema de la "papa caliente"
 *
 *  Se crea una ronda de n procesos (fork) que intercambian información
 *
 **/

#include <stdio.h>	// printf function
#include <unistd.h>	// _exit function
#include <stdlib.h>    // atoi function
#include <ctime>      // time function
#include <string.h>
#include <sys/wait.h>

#include "MailBox.h"  // MailBox class


struct message {
    long mtype;
    int mnums[2];
};

/**
  *  Aplica las reglas de Collatz al valor de la papa
  *
  *  Se simula el estallido de la papa cuando el valor retornado por esta función es uno
  *
 **/
int cambiarPapa( int papa ) {

   if ( 1 == (papa & 0x1) ) {			// ¿La papa es impar?
      papa = (papa << 1) + papa + 1;		// papa = papa * 2 + papa + 1 (papa * 3 + 1)
   } else {
      papa >>= 1;				// papa = papa / 2, utiliza corrimiento a la derecha, una posicion
   }

   return papa;

}


/**
 *   Procedimiento para simular una persona participante en la ronda
 *
 *   Recibe la identificación del buzón, la identificación de la persona y el sentido de rotación
 *
 **/

int persona(long id, int direction, int playerCount) {
   struct message msg;
   bool out = false;
   MailBox m;
   long next = ((id / 10) + direction) *10;
   if (next <= 0) {
      next = (playerCount) * 10;
   } else if (next > playerCount * 10) {
      next = 10;
   }
   while (true){
      m.receive( id, &msg, sizeof( msg.mnums)*2 );
      if (msg.mnums[0] == -1) {
            m.send( next, &msg, sizeof( msg.mnums)*2 );
         break;
      }
      if (!out) {
         msg.mnums[0] = cambiarPapa(msg.mnums[0]);
         printf("El jugador %ld tiene la papa %d\n", id / 10, msg.mnums[0]);
         printf("El siguiente jugador es %ld\n", next / 10);
         if (msg.mnums[0] == 1) {
           msg.mnums[1]++;
            out = true;
            msg.mnums[0] = 1 + rand() % 1000;
            printf("El jugador %ld se fue del juego\n",id / 10);
            printf("*Han salido %d jugadores de %d*\n",msg.mnums[1],playerCount);
         } else if (msg.mnums[1] == playerCount -1) {
            printf("El jugador %ld es el ganador\n", id / 10);
            msg.mnums[0] = -1;
         }
      }
      m.send( next, &msg, sizeof( msg.mnums)*2 );
   }
   _exit( 0 );	// Everything OK
}

/**
 *
 *   Procedimiento principal
 *
 **/
int main( int cantidad, char ** valores ) {
   long i;
   int st;
   int playerCount = 10;
   int vi = 2023;
   int direction = 1;

   if ( cantidad > 1 ) { // Define la cantidad de playerCount
      playerCount = atoi( valores[ 1 ] );
   }

   if ( cantidad > 2 ) { // Define el valor inicial de la papa
      vi = atoi( valores[ 2 ] );
   }

   if ( cantidad > 3 ) {	// Pueden utilizar la interpretación de este parámetro como mejor les convenga
      direction = atoi( valores[ 3 ] );
      if ( direction < 0 ) {
         direction = -1;		// El proceso i se la pasa al i - 1 (sugerencia)
      } else {
         direction = +1;		// El proceso i se la pasa al i + 1
      }
   }

   srand(time(NULL));	// Coloca una semilla para los números aleatorios

   struct message msg;
   msg.mnums[0] = vi;
   msg.mnums[1] = 0;
   MailBox m;

   printf( "Creando una ronda de %d jugadores\n", playerCount );

   do {					// Escoge el proceso que comienza el juego, definiendo el tipo de mensaje
      msg.mtype = (random() % playerCount) * 10;
   } while ( 0 == msg.mtype );
   printf( "El jugador %ld comienza el juego\n", msg.mtype / 10);
   m.send( msg.mtype, &msg, sizeof( msg.mnums)*2 );

   for (i = 1; i <= playerCount; i++ ) {
      if ( ! fork() ) {
         persona(i* 10, direction, playerCount);	// Este proceso simula una persona participante
      }
   }

   for ( i = 1; i <= playerCount; i++ ) {
      wait( &st );		// Esperar hasta que finalicen los procesos
   }
   // Elimina los recursos compartidos
   return 0;
}

