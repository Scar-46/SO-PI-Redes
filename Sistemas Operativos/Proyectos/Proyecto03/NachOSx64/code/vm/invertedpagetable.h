// invertedpagetable.h
// Saves references to the pages in physical memory
//
// Copyright (c) 2023 Oscar Mario Fernández Jiménez
// All rights reserved.  See copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include <stdio.h>
#include "bitmap.h"
#include "translate.h"

#ifndef INVERTED_PAGE_TABLE

class InvertedPageTable {
  public:
    InvertedPageTable();						// Initialize
    ~InvertedPageTable();						// De-allocate
    
    void addEntry(TranslationEntry * entry);		// Add an entry to the table
    void removeEntry(int pageFrame);			// Remove an entry from the table
    TranslationEntry * getEntry(int pageFrame);	// Get an entry from the table
    int searchVictim();							// Search for a victim

    void updateTLB(TranslationEntry * entry);	// Update the TLB
    void getFromSwap(int pageFrame);// Get an entry from the swap file
    void getFromExecutable(int pageFrame)                          ; // Get an entry from the executable file


    
  private:
    TranslationEntry * table[NumPhysPages];		// The table itself
    BitMap * tableMap;							// A bitmap to keep track of the entries
    int nextVictim;								// The next victim to be swapped out
};

#endif // INVERTED_PAGE_TABLE
