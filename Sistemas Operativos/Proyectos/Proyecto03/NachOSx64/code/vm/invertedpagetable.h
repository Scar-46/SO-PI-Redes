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

#include <iostream>
#include <vector>

#define NumPhysPages 32

#ifndef INVERTED_PAGE_TABLE
#define INVERTED_PAGE_TABLE

typedef struct {
    AddrSpace* threadSp;
    int virtualPage;
    int usage;
} Page;

class InvertedTable {
 public:
    InvertedTable(); // constructor
    ~InvertedTable(); // destructor

    int getPhysicalPage(int virtualPage); // returns physical page number or -1 if not found
    int searchPage(int page); // returns physical page number or -1 if not found
    int getLeastUsedPage(); // returns the least used page
    void updatePageUsage(int pageNum, bool reset); // updates the usage of a page
    void restorePages(); // restores all pages that belong to the thread
    
  private:
    BitMap* physicalMap; 
    Page* tableEntry;
};

#endif // INVERTED_PAGE_TABLE
