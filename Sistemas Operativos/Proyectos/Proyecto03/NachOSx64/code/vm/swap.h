// swap.h
//	Holds all the information about the swap file
//
// Copyright (c) 2023 Oscar Mario Fernández Jiménez
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "filesys.h"
#include "system.h"
#include <sstream>
#include <string>

#ifndef SWAP_H
#define SWAP_H

#define NUMBER_OF_PAGES 64
#define SWAP_FILE_NAME "SWAP_FILE"

class SwapFile {
  public:
    SwapFile(); // Constructor
    ~SwapFile(); // Destructor

    void writeToSwap(int vpn, int ppn); // Write a page to the swap file
    void readFromSwap(int vpn, int ppn); // Read a page from the swap file
    bool inSwap(int vpn); // Check if a page is in the swap file
    void resetSwap(); // Reset the swap file
  private:
    OpenFile* swapFile; // swap file
    BitMap* swapMap; // bitmap to store the pages in the swap file
};

#endif // SWAP_H