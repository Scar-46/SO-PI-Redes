#include "invertedpagetable.h"
#include "bitmap.h"
#include "addrspace.h"
#include "../machine/machine.h"
#include "../threads/system.h"

InvertedTable::InvertedTable(){
    this->tableEntry = new Page[NumPhysPages];
    this->physicalMap = new BitMap(NumPhysPages);
} 

InvertedTable::~InvertedTable() {
    delete this->physicalMap;
    delete[] this->tableEntry;
}

int InvertedTable::searchPage(int virtualPage) {
    // find an available page on the inverted table
    int physicalPage = this->physicalMap->Find();

    // if all pages used
    if (physicalPage == -1) {
        // find least used and set it to be replaced
        physicalPage = this->getLeastUsedPage();
        AddrSpace* space = this->tableEntry[physicalPage].threadSp;
        space->Swap(physicalPage, this->tableEntry[physicalPage].virtualPage); // swap page
    }
    // set virtual virtual page on vector
    this->tableEntry[physicalPage].virtualPage = virtualPage;
    this->tableEntry[physicalPage].threadSp = currentThread->space;

    return physicalPage;
}

void InvertedTable::updatePageUsage(int pageNum, bool reset) {
    if (reset) {
        this->tableEntry[pageNum].usage = 0;
    } else {
        if (this->tableEntry[pageNum].usage >= 2048) {
            this->tableEntry[pageNum].usage = 0;
        } else {
            this->tableEntry[pageNum].usage += 1;
        }
    }
}

int InvertedTable::getLeastUsedPage() {
    int lowestPos = 0;
    int tempUsage = 0;
    int lowestUsage = this->tableEntry[lowestUsage].usage;

    // for all pages, check which has the lower amount of calls
    for (int virtualPage = 1; virtualPage < NumPhysPages; virtualPage++) {
        tempUsage = this->tableEntry[virtualPage].usage;
        if (tempUsage < lowestUsage) {  // if the current page has less calls
            lowestPos = virtualPage;
            lowestUsage = tempUsage;
        }
    }
    this->tableEntry[lowestUsage].usage = 0;  // reset usage of the page

    return lowestPos;
}