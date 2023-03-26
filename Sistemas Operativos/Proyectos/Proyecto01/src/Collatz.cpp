#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

typedef struct public_msg {
   bool finished;
   int papa;
   int playerCount;
   int playersOut;
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

   while (true){
      sem_wait(&msgPublic->canPlay[msgPrivate->id]);
      if (msgPublic->finished) {
         sem_post(&msgPublic->canPlay[(msgPrivate->id + 1) % msgPublic->playerCount]);
         break;
      }
      msgPublic->papa = cambiarPapa(msgPublic->papa);
      if (msgPublic->papa == 1) {
         msgPublic->playersOut++;
         msgPrivate->out = true;
         printf("El jugador %d se fue del juego\n", msgPrivate->id);
      }
      if (msgPublic->playersOut == msgPublic->playerCount - 1) {
         printf("El jugador %d es el ganador\n", (msgPrivate->id + 1) % msgPublic->playerCount);
         msgPublic->finished = true;
      }
      //printf("Hola, soy el jugador %d\n", ((msgp_t*) data)->id);
      sem_post(&msgPublic->canPlay[(msgPrivate->id + 1) % msgPublic->playerCount]);
     
   }
   return NULL;
}

int main(int argc, char* argv[])  {
   int playerCount = 10;
   int vi = 2023;
   int direction = 1;
   if ( argc > 1 ) { // Define la cantidad de playerCount
      playerCount = atoi( argv[ 1 ] );
   }

   if ( argc > 2 ) { // Define el valor inicial de la papa
      vi = atoi( argv[ 2 ] );
   }

   if ( argc > 3 ) { // Pueden utilizar la interpretación de este parámetro como mejor les convenga
      direction = atoi( argv[ 2 ] );
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
   msg->finished = false;
   msg->papa = vi;
   msg->playerCount = playerCount;
   msg->playersOut = 0;
   msg->canPlay = (sem_t*) malloc(sizeof(sem_t) * playerCount);

   // Memoria privada
   msgp_t* msgp = (msgp_t*) malloc(sizeof(msgp_t) * playerCount);

   // Generamos un jugador al azar para comenzar con la papa caliente
   srand(time(NULL));
   int firstPlayer = rand() % playerCount;
   int semValue;

   // Creamos los hilos para cada jugador
   for (int i = 0; i < playerCount; i++) {
      // Inicializamos en 0 los semáforos para cada jugador (excepto el primero)
      if (i == firstPlayer) {
         semValue = 1;
      } else {
         semValue = 0;
      }
      sem_init(&msg->canPlay[i], /*pshared*/ 0, /*value*/ semValue);
      
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