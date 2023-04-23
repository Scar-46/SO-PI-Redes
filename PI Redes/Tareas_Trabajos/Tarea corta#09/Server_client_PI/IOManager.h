/**
 * @file IOManager.h
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

#ifndef IO_MANAGER
#define IO_MANAGER

#include <string>
#include <vector>

class IOManager {
 private:
    /* data */

    /**
     * @brief Judges is a given string is valid for use.
     * 
     * @param figure The string to be evaluated
     * @return true 
     * @return false 
     */
    bool isValid(std::string figure);

    /**
     * @brief Fixes a given string to match protocol (All lower case). 
     * 
     * @param figure The string to be fixed.
     */
    void fixFigure(std::string * figure);

 public:
    /**
     * @brief  Construct a new IOManager object
     * 
     */
    IOManager(/* args */);

    /**
     * @brief Destroy the IOManager object
     * 
     */
    ~IOManager();

    /**
     * @brief Asks the user to input the name of a figure and fixes bad inputs.
     * 
     * @return std::string The fixed figure.
     */
    std::string getFigure();

    /**
     * @brief Prints the response after the building process is done.
     * 
     * @param amounts vector with the amounts of each piece.
     * @param pieces vector with the pieces of Lego.
     */
    void printResponse(std::vector<int> * amounts,
        std::vector<std::string> * pieces);
};

#endif
