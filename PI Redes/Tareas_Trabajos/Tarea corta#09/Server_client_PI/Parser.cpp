/**
 * @file Parser.cpp
 * @copyright Copyright (c) 2023
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @author Omar Camacho Calvo - omar.camacho@ucr.ac.cr
 * @author Joseph Fernando Núñez Solano - joseph.nunezsolano@ucr.ac.cr
 * @author Pablo Rodríguez Navarro - pablo.rodrigueznavarro@ucr.ac.cr
 * @brief Cpp for the Parser class that reads pieces of Lego from a line of text
 * @version 0.1
 * @date 2023-04-04
 * 
 */

#include "Parser.h"
#include <iostream>

Parser :: Parser() {
}

Parser :: ~Parser() {
}

void Parser :: readLine(std::string line) {
  std::string line_copy = line;
  std::string piece;
  std::smatch matches;
  std::regex reg_exp("ALIGN=center> ([a-zA-Z]+ (\\w+ ?)*)");
  std::string match;
  int number;
  // int total = 0;
  std::regex reg_numbers("ALIGN=center>\\s*(\\d+)");
  std::string matches_String;
  std::string subPiece;

  // match every part of the figure
  while (std::regex_search(line_copy, matches, reg_exp)) {
    piece = matches.str(ZERO);
    subPiece = piece.substr(14, piece.length());
    pieces.push_back(subPiece);
    line_copy = matches.suffix().str();
  }

  // match all (ALIGN=center> \d+) matches
  while (std::regex_search(line, matches, reg_numbers)) {
    number = std::stoi(matches[1].str().c_str());
    amounts.push_back(number);
    line = matches.suffix().str();
  }
}

std::vector<int> * Parser::getAmounts() {
  return &this->amounts;
}

std::vector<std::string> * Parser::getPieces() {
  return &this->pieces;
}
