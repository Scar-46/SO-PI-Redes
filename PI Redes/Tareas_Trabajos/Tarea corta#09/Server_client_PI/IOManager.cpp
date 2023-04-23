/**
 * @file IOManager.cpp
 * @copyright Copyright (c) 2023
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @author Omar Camacho Calvo - omar.camacho@ucr.ac.cr
 * @author Joseph Fernando Núñez Solano - joseph.nunezsolano@ucr.ac.cr
 * @author Pablo Rodríguez Navarro - pablo.rodrigueznavarro@ucr.ac.cr
 * @brief Clase que maneja los inputs y outputs.
 * @version 0.1
 * @date 2023-04-10
 * 
 */

#include <ctype.h>
#include <iostream>
#include <fstream>
#include "IOManager.h"

IOManager::IOManager() {
}

IOManager::~IOManager() {
}

std::string IOManager::getFigure() {
  std::string figure;
  bool out = false;

  std::cout << "Bienvenido al programa para obtener las piezas de Lego."
    << std::endl;

  while (!out) {
    std::cout << "Digite el nombre de la figura que quiere armar: "
      << std::endl;
    std::cin >> figure;

    if (isValid(figure)) {
      fixFigure(&figure);
      out = true;
    } else {
      std::cout << "Non-valid input, please enter a figure using only letters."
        << std::endl;
    }
  }

  return figure;
}

bool IOManager::isValid(std::string figure) {
  bool valid = true;

  int f_size = figure.size();
  int i = 0;
  bool out = false;

  while (!out) {
    if (isalpha(figure[i])) {
      ++i;
    } else {
      valid = false;
      out = true;
    }

    if (i == f_size) {
      out = true;
    }
  }

  return valid;
}

void IOManager::fixFigure(std::string * figure) {
  int f_size = figure->size();

  for (int i = 0; i < f_size; ++i) {
    if (65 <= (*figure)[i] && (*figure)[i] <= 90) {
      (*figure)[i] += 32;
    }
  }
}

void IOManager::printResponse(std::vector<int> * amounts,
  std::vector<std::string> * pieces) {
  int total = 0;

  if (amounts->size() != 0) {
    std::cout << "Se encontraron " << amounts->size()
    << " piezas diferentes de Lego." << std::endl;
    std::cout << "Las piezas de Lego que se necesitan son: " << std::endl;
    for (int i = 0; i < static_cast<int>(amounts->size()); i++) {
      std::cout << "\t" << (*amounts)[i] << " : " << (*pieces)[i] << std::endl;
      total += (*amounts)[i];
    }
    std::cout << "El total de piezas necesitadas es de: " << total << std::endl;
  } else {
    std::ifstream file ("404_error_god.txt");
    std::string line;
    if (file.is_open()) {
      while (getline(file, line)) {
        std::cout << line << std::endl;
      }
      file.close();
    } else {
      std::cout << "Unable to open file";
    }
    std::cout << "\n*El valor ingresado no es válido o la figura no existe" <<
    std::endl;
    exit(0);
  }
}
