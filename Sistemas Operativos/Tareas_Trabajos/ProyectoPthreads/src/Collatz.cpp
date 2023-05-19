/**
 * @author Oscar Fernández Jiménez - C12840
 * @copyright Copyright (c) 2023
 */

#include <iostream>
#include "Collatz.h"

int cambiarPapa(int papa) {
  if (1 == (papa & 0x1)) {  // papa es impar
    papa = (papa << 1) + papa + 1;  // papa = papa * 2 + papa + 1
  } else {
    papa >>= 1;  // n = n / 2, utiliza corrimiento a la derecha, una posicion
  }
  return papa;
}

void* persona(void* data) {
  unsigned int seed = time(NULL);
  msgp_t* msgPrivate = reinterpret_cast<msgp_t*>(data);
  msg_t* msgPublic = msgPrivate->msg;
  int direction = msgPublic->direction;
  int nextPlayer = (msgPrivate->id + direction) % msgPublic->playerCount;
  if (nextPlayer < 0) {
    nextPlayer = msgPublic->playerCount - 1;
  }
  while (true) {
    sem_wait(&msgPublic->canPlay[msgPrivate->id]);
    if (msgPublic->papa == -1) {
      sem_post(&msgPublic->canPlay[nextPlayer]);
      break;
    }
    // Si el jugador no está fuera del juego
    if (!msgPrivate->out) {
        msgPublic->papa = cambiarPapa(msgPublic->papa);
        std::cout << "El jugador " << msgPrivate->id + 1
        << " tiene la papa " << msgPublic->papa << std::endl;
        if (msgPublic->papa == 1 &&
            msgPublic->playersOut < msgPublic->playerCount - 1) {
          msgPublic->playersOut++;
          msgPrivate->out = true;
          msgPublic->papa =  1 + rand_r(&seed) % 1000;
          std::cout << "El jugador " << msgPrivate->id + 1
          << " se fue del juego" << std::endl;
          std::cout << "*Han salido " << msgPublic->playersOut
          << " jugadores de "<< msgPublic->playerCount << "*" << std::endl;
        } else if (msgPublic->playersOut == msgPublic->playerCount -1) {
          std::cout << "El jugador " << msgPrivate->id + 1 << " es el ganador"
          << std::endl;
          msgPublic->papa = -1;
        }
    }
    sem_post(&msgPublic->canPlay[nextPlayer]);
  }
  return NULL;
}
