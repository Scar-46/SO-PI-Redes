/**
 * @file Collatz.cpp
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief Do all the operations to play the game
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <iostream>  // cout function
#include <unistd.h>	// _exit function

#include "Collatz.h"
#include "MailBox.h"

int cambiarPapa(int papa) {
  if ( 1 == (papa & 0x1) ) {
    papa = (papa << 1) + papa + 1;
  } else {
    papa >>= 1;
  }

  return papa;
}


int persona(long id, int direction, int playerCount) {
  unsigned int seed = time(NULL);
  msg_t msg;
  bool out = false;
  MailBox m;
  long next = ((id / 10) + direction) *10;
  if (next <= 0) {
    next = (playerCount) * 10;
  } else if (next > playerCount * 10) {
    next = 10;
  }
  while (true){
    // receives the message with the potato
    m.receive( id, &msg, sizeof( msg.mnums)*2 );
    if (msg.mnums[0] == -1) {
      // sends the stop message to the next player
      m.send( next, &msg, sizeof( msg.mnums)*2 );
      break;
    }
    // if the player is out, it will not receive the potato
    if (!out) {
      msg.mnums[0] = cambiarPapa(msg.mnums[0]);
      std::cout << "El jugador " << id / 10 << " tiene la papa " << msg.mnums[0] << std::endl;
      if (msg.mnums[0] == 1) {
        msg.mnums[1]++;
        out = true;
        msg.mnums[0] = 1 + rand_r(&seed) % 1000;
        std::cout << "El jugador " << id / 10 << " se fue del juego" << std::endl;
        std::cout << "*Han salido " << msg.mnums[1] << " jugadores de "
        << playerCount << "*" << std::endl;
      } else if (msg.mnums[1] == playerCount -1) {
        std::cout << "El jugador " << id / 10 << " es el ganador" << std::endl;
        msg.mnums[0] = -1;
      }
    }
    // sends the message to the next player with the new potato
    m.send( next, &msg, sizeof( msg.mnums)*2 );
  }
  _exit( 0 );	// Everything OK
}