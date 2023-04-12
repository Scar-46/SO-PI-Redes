/**
 * @file Collatz.h
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief  Do all the operations to play the game
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COLLATZ_H
#define COLLATZ_H

/**
 * @brief Change the potato's value
 * 
 * @param papa  The potato's original value
 * @return int  The potato's new value
 */
int cambiarPapa(int papa);

/**
 * @brief Simulate a player
 * 
 * @param id  Player's id
 * @param direction  Direction of the potato
 * @param playerCount  Number of players
 * @return int  0 if success
 */
int persona(long id, int direction, int playerCount);

#endif
