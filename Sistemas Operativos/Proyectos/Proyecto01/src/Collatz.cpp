/**
 * @file Collatz.cpp
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief  Source file for the Collatz functions
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <unistd.h>  // _exit function
#include <stdlib.h>  // rand function
#include <ctime>  // time function

#include "Collatz.h"

/**
  * @brief Aplly the rules of the game to the potato value
  * Simulates the rules of Collatz to the potato value
  * @param potato potato value
  * @return int new potato value
  *
 **/
int changePotato(int potato) {
  if ( 1 == (potato & 0x1) ) {  // is the potato odd?
    potato = (potato << 1) + potato + 1;  // p = p * 2 + p + 1 (p * 3 + 1)
  } else {
    potato >>= 1;  // potato = potato / 2, utilize the right shift operator
  }

  return potato;
}

/**
 * @brief  Simulates a round of the game of hot potato
 * Recives the semaphore vector, the id of the player and the direction of rotation
 * @param semsVector Semaphore vector
 * @param id buzon id
 * @param direction direction of rotation
 * @return int 0 if everything is ok
 *
 **/

int person(std::vector<sem_a_t> semsVector, int id, int direction, ShM& shmem) {
  bool out = false;
  unsigned int seed = time(NULL);
  msg_t* msgPublic = reinterpret_cast<msg_t*>(shmem.attach());
  int next = ((id-1) + direction) % msgPublic->playerCount;
  if (next < 0) {
    next = msgPublic->playerCount - 1;
  }
  // while player is not out
  while (true) {
    semsVector[id-1].canPlay->Wait();
    if (msgPublic->potato == -1) {
      semsVector[next].canPlay->Signal();
      break;
    }
    if (!out) {
      // change potato
      msgPublic->potato = changePotato(msgPublic->potato);
      printf("El jugador %d tiene la papa %d\n", id, msgPublic->potato);
      if (msgPublic->potato == 1 &&
          msgPublic->playersOut < msgPublic->playerCount - 1) {
        msgPublic->playersOut++;
        out = true;
        msgPublic->potato = 1 + rand_r(&seed) % 1000;
        printf("El jugador %d se fue del juego\n", id);
        printf("*Han salido %d jugadores de %d*\n", msgPublic->playersOut,
        msgPublic->playerCount);
      } else if (msgPublic->playersOut == msgPublic->playerCount -1) {
        printf("El jugador %d es el ganador\n", id);
        msgPublic->potato = -1;
      }
    }
    semsVector[next].canPlay->Signal();
  }
  _exit(0);  // Everything OK
}
