/**
 * @file papa.cpp 
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief  
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <iostream>   // cout function
#include <unistd.h>	// _exit function
#include <stdlib.h>    // atoi function
#include <ctime>      // time function
#include <string.h>
#include <sys/wait.h>

#include "MailBox.h"  // MailBox class
#include "Collatz.h"  // Collatz functions


/**
 * @brief Main function
 * 
 * @param cantidad  Number of arguments
 * @param valores  Arguments (playerCount, vi, direction)
 * @return int  0 if success
 */
int main( int cantidad, char ** valores ) {
  long i;
  int st;
  int playerCount = 100;
  int vi = 2023;
  int direction = 1;

  if (cantidad > 1) { // Defines the number of players
    playerCount = atoi( valores[1]);
  }

  if (cantidad > 2) { // Defines the initial value of the potato
    vi = atoi(valores[2]);
  }

  if ( cantidad > 3 ) {
    direction = atoi( valores[3]);
    if (direction < 0) {
      direction = -1;  // The process i passes the potato to i - 1
    } else {
      direction = +1;  // The process i passes the potato to i + 1
    }
  }

  srand(time(NULL)); // Initialize random seed

  struct message msg;
  msg.mnums[0] = vi;
  msg.mnums[1] = 0;
  MailBox m;
  std::cout << "Creando una ronda de " << playerCount << " jugadores" << std::endl;
  do {  // Select a random player to start the game
    msg.mtype = (random() % playerCount) * 10;
  } while ( 0 == msg.mtype );
  std::cout << "El jugador " << msg.mtype / 10 << " comienza el juego" << std::endl;
  m.send( msg.mtype, &msg, sizeof( msg.mnums)*2 );

  for (i = 1; i <= playerCount; i++ ) {
    if ( ! fork() ) {
      persona(i* 10, direction, playerCount); // Create a new process
    }
  }

  for ( i = 1; i <= playerCount; i++ ) {
    wait( &st ); // Wait for all the processes to finish
  }
  return 0;
}
