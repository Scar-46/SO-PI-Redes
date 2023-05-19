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
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>  // printf function
#include <unistd.h>  // _exit function
#include <stdlib.h>  // atoi function
#include <ctime>  // time function
#include <vector>

#include "Semaphore.h"  // Semaphore class
#include "ShM.h"  // Shared memory class
#include "Collatz.h"  // Collatz functions

/**
 * @brief Main function
 */
int main(int quantity, char ** values) {
  int i, st;
  msg_t* area;
  msg_t message;
  ShM shmem(1024);
  int playerCount = 100;
  int vi = 2023;
  int direction = 1;
  if (quantity > 1) {  // Define the number of participants
    playerCount = atoi(values[1]);
  }
  if (quantity > 2) {  // Define the initial value of the potato
    vi = atoi(values[2]);
  }
  if (quantity > 3) {  // Define the direction of rotation
    direction = atoi(values[3]);
    if (direction < 0) {
      direction = -1;  // The process i passes the potato to i - 1
    } else {
      direction = +1;  // The process i passes the potato to i + 1
    }
  }
  srand(time(NULL));  // Set the seed for the random number generator
  area = reinterpret_cast<msg_t*>(shmem.attach());
  message.potato = vi;
  message.playerCount = playerCount;
  message.playersOut = 0;
  std::vector<sem_a_t> semsVector;
  memcpy(area, &message, sizeof(msg_t));
  printf("Creando una ronda de %d playerCount\n", playerCount);
  int firstPlayer;
  do {  // Choose a random player to start the game
    firstPlayer = (random() % playerCount);
  } while ( 0 == firstPlayer );
  printf("El jugador %d comienza el juego\n", firstPlayer);
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
  for (i = 1; i <= playerCount; i++) {
    if (!fork()) {
      person(semsVector, i, direction, shmem);  // Call the player function
    }
  }

  for (i = 0; i < playerCount; i++) {
    wait(&st);  // Wait for the players processes
    delete semsVector[i].canPlay;  // Delete the semaphores
  }

  // Delete the shared memory
  shmem.detach();
  return 0;
}
