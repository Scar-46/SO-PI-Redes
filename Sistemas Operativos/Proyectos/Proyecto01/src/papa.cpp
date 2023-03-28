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
#include "Semaphore.h" // Semaphore class
#include "ShM.h"      // Shared memory class
#include <string.h>
#include <vector>
#include <sys/wait.h>

ShM shmem(1024);

typedef struct public_msg {
   int papa;
   int playerCount;
   int playersOut;
}msg_t;

typedef struct semaphores {
   Semaphore* canPlay;
}sem_a_t;

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

int persona(std::vector<sem_a_t> semsVector, int id, int sentido) {
   bool out = false;
   msg_t* msgPublic = (msg_t*)shmem.attach();
   
   while (true){
      msg_t* msgPublic = (msg_t*)shmem.attach();
      semsVector[id-1].canPlay->Wait();
      int next = ((id-1) + sentido) % msgPublic->playerCount;
      if (next < 0) {
         next = msgPublic->playerCount - 1;
      }
      if (msgPublic->papa == -1) {
         semsVector[next].canPlay->Signal();
         break;
      }
      if (!out) {
         msgPublic->papa = cambiarPapa(msgPublic->papa);
         if (msgPublic->papa == 1) {
            msgPublic->playersOut++;
            out = true;
            msgPublic->papa = 1 + rand() % 1000;
            printf("El jugador %d se fue del juego\n",id);
         }
         if (msgPublic->playersOut == msgPublic->playerCount) {
            printf("El jugador %d es el ganador\n", id);
            msgPublic->papa = -1;
         }
      }
      semsVector[next].canPlay->Signal();
   }
   
   shmem.detach();
   _exit( 0 );	// Everything OK

}

/**
 *
 *   Procedimiento principal
 *
 **/
int main( int cantidad, char ** valores ) {
   int i, id, st, tipo;
   msg_t* area;
   msg_t message;

   int participantes = 100;
   int vi = 2023;
   int sentido = 1;
   if ( cantidad > 1 ) { // Define la cantidad de playerCount
      participantes = atoi( valores[ 1 ] );
   }

   if ( cantidad > 2 ) { // Define el valor inicial de la papa
      vi = atoi( valores[ 2 ] );
   }

   if ( cantidad > 3 ) {	// Pueden utilizar la interpretación de este parámetro como mejor les convenga
      sentido = atoi( valores[ 3 ] );
      if ( sentido < 0 ) {
         sentido = -1;		// El proceso i se la pasa al i - 1 (sugerencia)
      } else {
         sentido = +1;		// El proceso i se la pasa al i + 1
      }
   }

      srand(time(NULL));	// Coloca una semilla para los números aleatorios

   area = (msg_t *) shmem.attach();
   message.papa = vi;
   message.playerCount = participantes;
   message.playersOut = 0;
   std::vector<sem_a_t> semsVector;
   memcpy(area, &message, sizeof(msg_t));

   printf( "Creando una ronda de %d participantes\n", participantes );

   int firstPlayer;
   do {					// Escoge el proceso que comienza el juego, definiendo el tipo de mensaje
      firstPlayer = (random() % participantes);
   } while ( 0 == firstPlayer );
   printf( "El jugador %d comienza el juego\n", firstPlayer);

   for (i = 1; i <= participantes; i++) {
      sem_a_t sem;
      if (i == firstPlayer) {
         sem.canPlay = new Semaphore(i, 1);
      } else {
         sem.canPlay = new Semaphore(i, 0);
      }
      semsVector.push_back(sem);
   }

   for (i = 1; i <= participantes; i++ ) {
      if ( ! fork() ) {
         persona(semsVector, i, sentido );	// Este proceso simula una persona participante
      }
   }

   for ( i = 1; i <= participantes; i++ ) {
      wait( &st );		// Esperar hasta que finalicen los procesos
   }

// Elimina los recursos compartidos
   shmem.detach();
   for (i = 0; i < participantes; i++) {
      delete semsVector[i].canPlay;
   }

   return 0;
}

