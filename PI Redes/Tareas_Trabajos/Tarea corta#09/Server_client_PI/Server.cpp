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
#include <fstream>
#include <regex>
#include <vector>
#include <unistd.h>

#include "Parser.h"
#include "Socket.h"
#include "IOManager.h"

#define PORT 3141
#define BUFSIZE 1024

//TODO(ANY): Error handling

std::string checkPieces(char *a){
  std::string request = a;
  std::string response;
  int initialP = request.find("figure=");
  int finalP = request.find(" HTTP/");
  std::string figure = request.substr(initialP+7, finalP-initialP-7);
  std::ifstream file ("figures/" + figure + ".txt");
  std::string line;
  if (file.is_open()){
    while (getline(file, line)) {
      response += "\n" + line;
    }
    file.close();
  }else {
    printf("No se pudo abrir el archivo");
  }
  return response;
}

int main(int argc, char * argv[]) {
  if (argc == 2) {
    //TODO(ANY): Client that requests the all the figures
  } else {
    int childpid = 1;
    char a[ BUFSIZE ];
    Socket sPieces('s'), *sPiecesP;

    sPieces.Bind( 3141 );			// Port to access this mirror server
    sPieces.Listen( 5 );			// Set backlog queue to 5 conections

    while (true) {		// Loop forever (until ctrl-c)
      sPiecesP = sPieces.Accept();	 		// Wait for a new conection, conection info is in sPiecesP variable
      childpid = fork();		// Create a child to serve the request
      if ( childpid < 0 ) {
        perror( "server: fork error" );
      } else {
        if (0 == childpid) {		// child code
        sPieces.Close();			// Close original socket "serverPieces" in child
        memset( a, 0, BUFSIZE );
        sPiecesP->Read( a, BUFSIZE );	// Read a string from client using new conection info
        std::string response = checkPieces(a);
        strcpy(a, response.c_str());
        sPiecesP->Write(a);	// Send response to client
        exit( 0 );			// Exit, finish child work
        }
      }
      //TODO(ANY): Make a signal handler
      sPiecesP->Close();			// Close socket sPiecesP in parent, then go wait for a new conection
    }
  }

  return 0;
}
