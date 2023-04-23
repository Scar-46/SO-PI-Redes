/**
 * @file Parser.h
 * @copyright Copyright (c) 2023
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @author Omar Camacho Calvo - omar.camacho@ucr.ac.cr
 * @author Joseph Fernando Núñez Solano - joseph.nunezsolano@ucr.ac.cr
 * @author Pablo Rodríguez Navarro - pablo.rodrigueznavarro@ucr.ac.cr
 * @brief  Header file for the Parser class that reads pieces of Lego from a line of text.
 * @version 0.1
 * @date 2023-04-04
 * 
 */

#include <stdlib.h>
#include <string>
#include <vector>
#include <regex>  // NOLINT
#include <utility>

#define ZERO 0

class Parser {
 public:
  /**
   * @brief Construct a new Parser object
   * 
   */
  Parser();

  /**
   * @brief Destroy the Parser object
   * 
   */
  ~Parser();

  /**
   * @brief Read the line and store the pieces of Lego
   * 
   * @param line  String with the line to be read
   */
  void readLine(std::string);

  /**
   * @brief Get the Amounts object
   * 
   * @return std::vector<int>* 
   */
  std::vector<int> * getAmounts();

  /**
   * @brief Get the Pieces object
   * 
   * @return std::vector<std::string>* 
   */
  std::vector<std::string> * getPieces();

 private:
  /// @brief Vector with the amounts of each piece
  std::vector<int> amounts;
  /// @brief Vector with the pieces of Lego
  std::vector<std::string> pieces;
};
