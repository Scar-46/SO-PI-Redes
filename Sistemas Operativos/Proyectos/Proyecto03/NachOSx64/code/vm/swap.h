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

#define NUMBER_OF_PAGES 128
#define SWAP_FILE_NAME "SWAP_FILE"

class SwapFile {
  public:
    SwapFile(); // Constructor
    ~SwapFile(); // Destructor

    void writeToSwapSpace(int vpn, int ppn, int threadID); // Write a page to the swap file
    void readFromSwapSpace(int vpn, int ppn, int threadID); // Read a page from the swap file
    bool inSwapSpace(int vpn, int threadID); // Check if a page is in the swap file
  private:
    OpenFile * swapFile;
    bool swapMap[NUMBER_OF_PAGES];
    int threadMap[NUMBER_OF_PAGES];
};

#endif // SWAP_H