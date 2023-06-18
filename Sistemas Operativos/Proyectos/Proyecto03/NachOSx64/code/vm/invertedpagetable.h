// invertedpagetable.h
// Saves references to the pages in physical memory
//
// Copyright (c) 2023 Oscar Mario Fernández Jiménez
// All rights reserved.  See copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "bitmap.h"
#include "machine.h"
#include "addrspace.h"
#include "thread.h"
#include "translate.h"
#include <iostream>
#include <vector>

#define NumPhysPages 32

#ifndef INVERTED_PAGE_TABLE
#define INVERTED_PAGE_TABLE


class InvertedTable {
 public:
    InvertedTable(); // constructor
    ~InvertedTable(); // destructor

    int findPage(int virtualPage, int processID); // returns physical page number
    int requestPage(int virtualPage, int processID); // allocates a physical page
    int getLeastUsedPage(); // returns the least used page
    void swap(int physicalPage, int virtualPage); // swaps a page
    void moveIntoTLB(TranslationEntry* tableToReplace, int virtTablePos);  // moves a page into the TLB
    void savePageState(int physicalPage, TranslationEntry* TLBEntry);  // saves the state of a page on the TLB to the inverted table

  private:
    BitMap* physicalMap; 
    TranslationEntry* tableEntry;
    int* processID;
    int currentVictim;
};

#endif // INVERTED_PAGE_TABLE
