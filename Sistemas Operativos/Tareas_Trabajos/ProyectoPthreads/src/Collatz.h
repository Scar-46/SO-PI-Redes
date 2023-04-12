/** 
 * @brief  Realiza las operaciones necesarias para el juego de la papa caliente
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @version 2023/Mar/15
 * 
 * @copyright Copyright (c) 2023
 * 
 *
 **/

#ifndef COLLATZ_H
#define COLLATZ_H

#include <semaphore.h>

/**
 * @brief Contiene el mensaje publico utilizado por el programa
 * @struct msg_t
 */
typedef struct public_msg {
  /// @brief Valor de la papa caliente
  int papa;
  /// @brief Cantidad de jugadores
  int playerCount;
  /// @brief Cantidad de jugadores que han salido del juego
  int playersOut;
  /// @brief Dirección de la papa caliente
  int direction;
  /// @brief Semaforos para cada jugador
  sem_t* canPlay;
}msg_t;

/**
 * @brief Contiene el mensaje privado utilizado por cada jugador
 * @struct msgp_t
 */
typedef struct private_msg {
  /// @brief Identificador del jugador
  int id;
  /// @brief Jugador fuera del juego
  bool out;
  /// @brief Mensaje publico
  msg_t* msg;
}msgp_t;

/**
 * @brief Cambia el valor de la papa caliente
 * 
 * @param papa  Valor de la papa caliente
 * @return int nuevo valor de la papa caliente
 */
int cambiarPapa(int papa);

/**
 * @brief Simula el comportamiento de un jugador
 * 
 * Recibe la papa caliente, la procesa y la pasa al siguiente jugador
 * 
 * @param data  Mensaje privado del jugador (id, msg y out)
 * @return void*  NULL
 */
void* persona(void* data);

#endif  // COLLATZ_H
