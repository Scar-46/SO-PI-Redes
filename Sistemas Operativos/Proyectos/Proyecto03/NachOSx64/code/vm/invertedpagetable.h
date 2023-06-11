// invertedpagetable.h
// Saves references to the pages in physical memory
//
// Copyright (c) 2023 Oscar Mario Fernández Jiménez
// All rights reserved.  See copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include <stdio.h>
#include "bitmap.h"
#include "translate.h"
#include "machine.h"
#include "swap.h"

#define NumPhysPages 32

#ifndef INVERTED_PAGE_TABLE
#define INVERTED_PAGE_TABLE


typedef struct {
  int threadID;
  TranslationEntry * entry;
} IPTEntry;


class InvertedPageTable {
  public:
    InvertedPageTable();						// Initialize
    ~InvertedPageTable();						// De-allocate

    int allocateFrame(TranslationEntry * entry, int threadID);	// Allocate a frame for a page
    void deallocateFrame(int frame);					// Deallocate a frame
    int getFrame(TranslationEntry * entry, int threadID);	// Get the frame of a page
    int searchVictim();							// Search for a victim to swap out
    int checkSwapFile(int vpn, int threadID);				// Check if a page is in the swap file
    void swapIn(int vpn, int ppn, int threadID);			// Swap in a page
    
  private:
    IPTEntry * table;							// The table itself
    BitMap * tableMap;							// A bitmap to keep track of the entries
    int nextVictim;								// The next victim to be swapped out
    // Swap file for swapping pages
    SwapFile * swapFile;
};

#endif // INVERTED_PAGE_TABLE
