/**
 * @file Collatz.cpp
 * @author Oscar Fernández Jiménez - C12840
 * @brief 
 * @version 0.1
 * @date 2023-03-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

typedef struct public_msg {
   int papa;
   int playerCount;
   int playersOut;
   int direction;
   sem_t* canPlay;
}msg_t;

typedef struct private_msg {
   int id;
   bool out;
   msg_t* msg;
}msgp_t;


int cambiarPapa(int papa) {
   if (1 == (papa & 0x1)) {		// papa es impar
      papa = (papa << 1) + papa + 1;	// papa = papa * 2 + papa + 1
   } else {
      papa >>= 1;			// n = n / 2, utiliza corrimiento a la derecha, una posicion
   }
   return papa;
}

void* persona(void* data) {
   msgp_t* msgPrivate = (msgp_t*) data;
   msg_t* msgPublic = msgPrivate->msg;
   int direction = msgPublic->direction;
   int nextPlayer = (msgPrivate->id + direction) % msgPublic->playerCount;
   if (nextPlayer < 0) {
      nextPlayer = msgPublic->playerCount - 1;
   }
   while (true){
      sem_wait(&msgPublic->canPlay[msgPrivate->id]);
      if (msgPublic->papa == -1) {
         sem_post(&msgPublic->canPlay[nextPlayer]);
         break;
      }
      if (!msgPrivate->out) {
         msgPublic->papa = cambiarPapa(msgPublic->papa);
         printf("El jugador %d tiene la papa %d\n",msgPrivate->id +1, msgPublic->papa);
         if (msgPublic->papa == 1) {
            msgPublic->playersOut++;
            msgPrivate->out = true;
            msgPublic->papa = 1 + rand() % 1000;
            printf("El jugador %d se fue del juego\n",msgPrivate->id +1);
            printf("*Han salido %d jugadores de %d*\n", msgPublic->playersOut, msgPublic->playerCount);
         } else if (msgPublic->playersOut == msgPublic->playerCount -1) {
            printf("El jugador %d es el ganador\n", msgPrivate->id +1);
            msgPublic->papa = -1;
         }
      }
      sem_post(&msgPublic->canPlay[nextPlayer]);
   }
   return NULL;
}

int main(int argc, char* argv[])  {
   int playerCount = 100;
   int vi = 2023;
   int direction = 1;
   if ( argc > 1 ) { // Define la cantidad de playerCount
      playerCount = atoi( argv[ 1 ] );
   }

   if ( argc > 2 ) { // Define el valor inicial de la papa
      vi = atoi( argv[ 2 ] );
   }

   if ( argc > 3 ) { // Pueden utilizar la interpretación de este parámetro como mejor les convenga
      direction = atoi( argv[ 3 ] );
      if ( direction < 0 ) {
         direction = -1;      // El proceso i se la pasa al i - 1 (sugerencia)
      } else {
         direction = +1;      // El proceso i se la pasa al i + 1
      }
   }

   // Memoria compartida
   pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * playerCount);
   msg_t* msg = (msg_t*) malloc(sizeof(msg_t));

   // Inicializamos la memoria compartida
   msg->papa = vi;
   msg->playerCount = playerCount;
   msg->playersOut = 0;
   msg->canPlay = (sem_t*) malloc(sizeof(sem_t) * playerCount);
   msg->direction = direction;

   // Memoria privada
   msgp_t* msgp = (msgp_t*) malloc(sizeof(msgp_t) * playerCount);

   // Generamos un jugador al azar para comenzar con la papa caliente
   srand(time(NULL));
   int firstPlayer = rand() % playerCount;
   int semValue;
   for (int i = 0; i < playerCount; i++) {
      // Inicializamos en 0 los semáforos para cada jugador (excepto el primero)
      if (i == firstPlayer) {
         semValue = 1;
      } else {
         semValue = 0;
      }
      sem_init(&msg->canPlay[i], /*pshared*/ 0, /*value*/ semValue);
   }

   // Creamos los hilos para cada jugador
   for (int i = 0; i < playerCount; i++) {
      // Inicializamos la memoria privada
      msgp[i].id = i;
      msgp[i].msg = msg;
      msgp[i].out = false;
      pthread_create(&threads[i], NULL, persona, (void*) &msgp[i]);
   }

   // Esperamos a que los hilos terminen
   for (int i = 0; i < playerCount; i++) {
      pthread_join(threads[i], NULL);
      sem_destroy(&msg->canPlay[i]);
   }

   // Liberamos la memoria
   free(msgp);
   free(msg->canPlay);
   free(msg);
   free(threads);

   return 0;
}