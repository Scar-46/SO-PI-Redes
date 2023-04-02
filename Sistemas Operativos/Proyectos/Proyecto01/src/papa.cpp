/**
 * @file potato.cpp 
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief  Program that simulates the game of hot potato
 * @version 0.1
 * @date 2023-04-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

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

/**
 * @brief Contains the message used by the program
 * @struct msg_t
 */
typedef struct public_msg {
   int potato;
   int playerCount;
   int playersOut;
}msg_t;

/**
 * @brief Contains the semaphores used by the program
 * @struct sem_a_t
 */
typedef struct semaphores {
   Semaphore* canPlay;
}sem_a_t;

/**
  * Aplly the rules of the game to the potato value
  * Simulates the rules of Collatz to the potato value
  * @param potato potato value
  *
 **/
int changePotato( int potato ) {

   if ( 1 == (potato & 0x1) ) {			// is the potato odd?
      potato = (potato << 1) + potato + 1;		// potato = potato * 2 + potato + 1 (potato * 3 + 1)
   } else {
      potato >>= 1;				// potato = potato / 2, utilize the right shift operator
   }

   return potato;

}

/**
 *  @brief  Simulates a round of the game of hot potato
 *  Recives the semaphore vector, the id of the player and the direction of rotation
 * @param semsVector Semaphore vector
 * @param id buzon id
 * @param direction direction of rotation
 *
 **/

int persona(std::vector<sem_a_t> semsVector, int id, int direction) {
   bool out = false;
   msg_t* msgPublic = (msg_t*)shmem.attach();
   int next = ((id-1) + direction) % msgPublic->playerCount;
   if (next < 0) {
      next = msgPublic->playerCount - 1;
   }
   // while player is not out
   while (true){
      semsVector[id-1].canPlay->Wait();
      if (msgPublic->potato == -1) {
         semsVector[next].canPlay->Signal();
         break;
      }
      if (!out) {
         // change potato
         msgPublic->potato = changePotato(msgPublic->potato);
         printf("El jugador %d tiene la papa %d\n", id, msgPublic->potato);
         if (msgPublic->potato == 1) {
            msgPublic->playersOut++;
            out = true;
            msgPublic->potato = 1 + rand() % 1000;
            printf("El jugador %d se fue del juego\n",id);
            printf("*Han salido %d jugadores de %d*\n", msgPublic->playersOut, msgPublic->playerCount);
         } else if (msgPublic->playersOut == msgPublic->playerCount -1) {
            printf("El jugador %d es el ganador\n", id);
            msgPublic->potato = -1;
         }
      }
      semsVector[next].canPlay->Signal();
   }
   _exit( 0 );	// Everything OK

}

/**
 *
 *   Principal function
 *
 **/
int main( int quantity, char ** values ) {
   int i, st;
   msg_t* area;
   msg_t message;

   int playerCount = 5;
   int vi = 2023;
   int direction = 1;
   if ( quantity > 1 ) { //Define the number of participants
      playerCount = atoi( values[ 1 ] );
   }

   if ( quantity > 2 ) { // Define the initial value of the potato
      vi = atoi( values[ 2 ] );
   }

   if ( quantity > 3 ) {	// Define the direction of rotation
      direction = atoi( values[ 3 ] );
      if ( direction < 0 ) {
         direction = -1;		// The process i passes the potato to i - 1
      } else {
         direction = +1;		// The process i passes the potato to i + 1
      }
   }

   srand(time(NULL));	// Set the seed for the random number generator

   area = (msg_t *) shmem.attach();
   message.potato = vi;
   message.playerCount = playerCount;
   message.playersOut = 0;
   std::vector<sem_a_t> semsVector;
   memcpy(area, &message, sizeof(msg_t));

   printf( "Creando una ronda de %d playerCount\n", playerCount );

   int firstPlayer;
   do {					// Choose a random player to start the game
      firstPlayer = (random() % playerCount);
   } while ( 0 == firstPlayer );
   printf( "El jugador %d comienza el juego\n", firstPlayer);

   // Create the semaphores
   for (i = 1; i <= playerCount; i++) {
      sem_a_t sem;
      if (i == firstPlayer) {
         sem.canPlay = new Semaphore(i, 1);
      } else {
         sem.canPlay = new Semaphore(i, 0);
      }
      semsVector.push_back(sem);
   }

   // Create the players processes
   for (i = 1; i <= playerCount; i++ ) {
      if ( ! fork() ) {
         persona(semsVector, i, direction );	// Create the players processes
      }
   }

   for ( i = 0; i < playerCount; i++ ) {
      wait( &st );		// Wait for the players processes
      delete semsVector[i].canPlay; // Delete the semaphores
   }

   // Delete the shared memory
   shmem.detach();
   return 0;
}

