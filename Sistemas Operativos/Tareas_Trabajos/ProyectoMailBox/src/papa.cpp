/**
 *  CI0122 Sistemas Operativos
 *  Grupo 2, 2023-i
 *
 *  Problema de la "papa caliente"
 *
 *  Se crea una ronda de n procesos (fork) que intercambian información
 *
 **/

#include <stdio.h>   // printf function
#include <stdlib.h>  // exit function
#include <unistd.h>  // _exit function
#include <ctime>  // time function
#include <condition_variable> // std::condition_variable
#include "MailBox.h"   // MailBox class
#include<sys/wait.h>

typedef struct info {
   int papa;
   int jugadoresFuera;
} info_t;

struct msgbuf {         // Estructura ejemplo para el intercambio de mensajes
   long mtype;
   info_t info;      // Almacena el valor de la papa
   // Pueden agregar todos los datos que consideren necesarios
};


 

/**
  *  Aplica las reglas de Collatz al valor de la papa
  *
  *  Se simula el estallido de la papa cuando el valor retornado por esta función es uno
  *
 **/
int cambiarPapa( int papa ) {

   if ( 1 == (papa & 0x1) ) {       // ¿La papa es impar?
      papa = (papa << 1) + papa + 1;      // papa = papa * 2 + papa + 1 (papa * 3 + 1)
   } else {
      papa >>= 1;          // papa = papa / 2, utiliza corrimiento a la derecha, una posicion
   }

   return papa;

}

/**
 *   Procedimiento para simular una persona participante en la ronda
 *
 *   Recibe la identificación del buzón, la identificación de la persona y el sentido de rotación
 *
 **/
int persona(MailBox buzon, int id, int sentido, int participantes) {
    info_t info;
    int hola;
    int papa;
    int tipo;
    bool out = false;
    while (1) {
         //printf("Persona %d ha entrado\n", id);
         buzon.receive(id, &hola, sizeof(int));  // Recibe el mensaje del participante anterior
         papa = info.papa;
         tipo = (id + sentido) % participantes;  // Calcula el siguiente participante
         printf("Persona %d recibió la papa %d\n", id, hola);
         if (papa == -1) {  // Condición de salida para detener el juego
            buzon.send(tipo, &info, sizeof(int));  // Envía el mensaje al siguiente participante
            break;
         }
         if (!out) {
            printf("Persona %d recibió la papa %d\n", id, papa);
            papa = cambiarPapa(papa);  // Aplica las reglas de Collatz a la papa
            if (papa == 1) {  // Si la papa es 1, entonces la persona ha perdido
               printf("Persona %d ha perdido\n", id);
               out = true;
               info.jugadoresFuera++;  // Incrementa el número de jugadores que han perdido
            }
         }
         info.papa = papa;
         buzon.send(tipo, &hola, sizeof(int));  // Envía el mensaje al siguiente participante
      }
    _exit(0);  // Todo ha terminado correctamente
}

/**
 *
 *   Procedimiento principal
 *
 **/
int main( int cantidad, char ** valores ) {
    int i, j, tipo;
    int participantes = 10; // Valor predefinido para la cantidad de participantes
    int vi = 2023; // Valor predefinido para la papa
    int sentido = -1; // Valor predefinido para el sentido de rotación
    int st;
   MailBox buzon;

   if ( cantidad > 1 ) {   // Define la cantidad de participantes
      participantes = atoi( valores[ 1 ] );
      if ( participantes <= 0 ) {
         participantes = 100;
      }
   }

   if ( cantidad > 2 ) {   // Define el valor inicial de la papa
      vi = atoi( valores[ 2 ] );
      if ( vi <= 0 ) {
         vi = 2023;
      }
   }

   if ( cantidad > 3 ) {   // Pueden utilizar la interpretación de este parámetro como mejor les convenga
      sentido = atoi( valores[ 2 ] );
      if ( sentido < 0 ) {
         sentido = -1;     // El proceso i se la pasa al i - 1 (sugerencia)
      } else {
         sentido = +1;     // El proceso i se la pasa al i + 1
      }
   }

   srandom( time( 0 ) );   // Coloca una semilla para los números aleatorios

   printf( "Creando una ronda de %d participantes\n", participantes );
   for ( i = 1; i <= participantes; i++ ) {
      if ( ! fork() ) {
         persona( buzon, i , sentido, participantes);   // Este proceso simula una persona participante
      }
   }

   do {              // Escoge el proceso que comienza el juego, definiendo el tipo de mensaje
      tipo = (random() % participantes) ;
   } while ( 0 == tipo );
   info_t info;
   int hola = 3;
   info.papa = vi;
   info.jugadoresFuera = 0;

   printf( "Receptor del primer mensaje %d, valor de la papa %d\n",tipo, info.papa );
   buzon.send(tipo, &hola, sizeof(int));

   for ( i = 1; i <= participantes; i++ ) {
      j = wait( &st );     // Esperar hasta que finalicen los procesos
   }

   printf( "Main: valor de la papa %d\n", info.papa );

// Elimina los recursos compartidos
   return 0;
}
