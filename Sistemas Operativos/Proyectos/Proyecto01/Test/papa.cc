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

int participantes = 100;	// Valor predefinido para la cantidad de participantes

struct msgbuf {			// Estructura ejemplo para el intercambio de mensajes
   long mtype;
   int papa;			// Almacena el valor de la papa
   // Pueden agregar todos los datos que consideren necesarios
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
int persona( int buzon, int id, int sentido ) {

   _exit( 0 );	// Everything OK

}


/**
 *
 *   Procedimiento principal
 *
 **/
int main( int cantidad, char ** valores ) {
   int i, id, st, tipo;
   int participantes, vi, sentido;
   MailBox buzon;

   if ( cantidad > 1 ) {	// Define la cantidad de participantes
      participantes = atoi( valores[ 1 ] );
      if ( participantes <= 0 ) {
         participantes = 100;
      }
   }

   if ( cantidad > 2 ) {	// Define el valor inicial de la papa
      vi = atoi( valores[ 2 ] );
      if ( vi <= 0 ) {
         vi = 2023;
      }
   }

   if ( cantidad > 3 ) {	// Pueden utilizar la interpretación de este parámetro como mejor les convenga
      sentido = atoi( valores[ 2 ] );
      if ( sentido < 0 ) {
         sentido = -1;		// El proceso i se la pasa al i - 1 (sugerencia)
      } else {
         sentido = +1;		// El proceso i se la pasa al i + 1
      }
   }

   srandom( time( 0 ) );	// Coloca una semilla para los números aleatorios

   printf( "Creando una ronda de %d participantes\n", participantes );
   for ( i = 1; i <= participantes; i++ ) {
      if ( ! fork() ) {
         persona( buzon, i * 10, sentido );	// Este proceso simula una persona participante
      }
   }


   do {					// Escoge el proceso que comienza el juego, definiendo el tipo de mensaje
      tipo = (random() % participantes) * 10;
   } while ( 0 == tipo );

   printf( "Receptor del primer mensaje %d, valor de la papa %d\n", m.mtype, m.papa );

   buzon.send( tipo, vi );

   for ( i = 1; i <= participantes; i++ ) {
      j = wait( &st );		// Esperar hasta que finalicen los procesos
   }

   printf( "Main: valor de la papa %d\n", m.papa );

// Elimina los recursos compartidos

}

