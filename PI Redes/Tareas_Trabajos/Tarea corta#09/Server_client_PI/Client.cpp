/**
 * @file main.cc
 * @copyright Copyright (c) 2023
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @author Omar Camacho Calvo - omar.camacho@ucr.ac.cr
 * @author Joseph Fernando Núñez Solano - joseph.nunezsolano@ucr.ac.cr
 * @author Pablo Rodríguez Navarro - pablo.rodrigueznavarro@ucr.ac.cr
 * @brief  Main file for the Lego project.
 * @version 0.1
 * @date 2023-04-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <regex>
#include <vector>

#include "Parser.h"
#include "Socket.h"
#include "IOManager.h"

#define SIZE 1024

int main(int argc, char * argv[]) {
  (void)argc;
  (void)argv;

  IOManager manager;

  std::string figure = manager.getFigure();

  // Concatenate figure to the url
  const char * os =  "127.0.0.1";
  std::string req = "GET /lego/list.php?figure=" + figure +
    " HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";
  char * reqOS =
    const_cast<char *>(reinterpret_cast<const char *>(req.c_str()));

  // Create a new stream socket for IPv6
  Socket s('s', false);
  char a[ SIZE ];
  int count;

  memset(a, 0, SIZE);

  // initialize de ssl en el socket
  s.Connect(os, 3141);
  s.Write(reqOS, strlen(reqOS));

  // TO-DO
  // check http-code for if/else in I/O response
  // maybe fix in line printResponse method - Pablo

  // read each line and parse it
  std::string buffer = "";
  std::string subBuffer = "";
  size_t pos = 0;
  Parser p;
  while ((count = s.Read(a, SIZE)) > 0) {
    buffer += a;
    while (true) {
      pos = buffer.find('\n');
      if (pos == std::string::npos) {
        break;
      }
      subBuffer = buffer.substr(0, pos);
      p.readLine(subBuffer);
      buffer.erase(0, pos + 2);
    }
    memset(a, 0, SIZE);
  }

  // Print the response
  manager.printResponse(p.getAmounts(), p.getPieces());
  return 0;
}

