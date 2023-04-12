/**
 * @file Collatz.h
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief  Header file for the Collatz functions 
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COLLATZ_H_
#define COLLATZ_H_

#include <vector>  // vector class

#include "Semaphore.h"  // Semaphore class
#include "ShM.h"  // Shared memory class

/**
  * Aplly the rules of the game to the potato value
  * Simulates the rules of Collatz to the potato value
  * @param potato  actual potato's value
  * @return int  new potato's value
  *
 **/
int changePotato(int potato);

/**
 *  @brief  Simulates a round of the game of hot potato
 *  Recives the semaphore vector, the id of the player and the direction of rotation
 * @param semsVector Semaphore vector
 * @param id buzon id
 * @param direction direction of rotation
 * @return int 0 if everything is ok
 *
 **/
int person(std::vector<sem_a_t> semsVector, int id, int direction, ShM& shmem);

#endif  // COLLATZ_H_
