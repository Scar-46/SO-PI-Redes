#include "invertedpagetable.h"
#include "bitmap.h"
#include "addrspace.h"
#include "../machine/machine.h"
#include "../threads/system.h"

InvertedTable::InvertedTable(){
    this->tableEntry = new TranslationEntry[NumPhysPages];
    this->physicalMap = new BitMap(NumPhysPages);
    this->processID = new int[NumPhysPages];
    for (int i = 0; i < NumPhysPages; i++) {
        this->tableEntry[i].virtualPage = -1;
        this->tableEntry[i].physicalPage = i;
        this->tableEntry[i].valid = false;
        this->tableEntry[i].use = false;
        this->tableEntry[i].dirty = false;
        this->tableEntry[i].readOnly = false;
        this->processID[i] = -1;
    }
} 

InvertedTable::~InvertedTable() {
    delete this->physicalMap;
    delete[] this->tableEntry;
    delete[] this->processID;
}

int InvertedTable::findPage(int virtualPage, int processID) {
    // find the page on the inverted table
    for (int i = 0; i < NumPhysPages; i++) {
        if (this->tableEntry[i].virtualPage == virtualPage && this->processID[i] == processID) {
            return i;
        }
    }
    return -1;
}

int InvertedTable::requestPage(int virtualPage, int processID) {
    // find an available page on the inverted table
    int physicalPage = this->physicalMap->Find();
    // if all pages used
    if (physicalPage == -1) {
        // find least used and set it to be replaced
        physicalPage = this->getLeastUsedPage();
        if (this->tableEntry[physicalPage].dirty) {
            // if dirty, swap
            this->swap(physicalPage, this->tableEntry[physicalPage].virtualPage);
        }
    }
    // set virtual virtual page on vector
    this->tableEntry[physicalPage].virtualPage = virtualPage;
    this->processID[physicalPage] = processID;
    return physicalPage;
}

void InvertedTable::swap(int physicalPage, int virtualPage) {
    // remove from TLB
    for (int entry = 0; entry < TLBSize; entry++) {
        if (machine->tlb[entry].virtualPage == virtualPage) {
            machine->tlb[entry].virtualPage = -1;
            machine->tlb[entry].physicalPage = -1;
            machine->tlb[entry].valid = false;
            machine->tlb[entry].use = false;
            machine->tlb[entry].dirty = false;
        }
    }
	// remove from page table
    this->tableEntry[physicalPage].virtualPage = -1;
    this->tableEntry[physicalPage].valid = false;
    this->tableEntry[physicalPage].use = false;
    this->tableEntry[physicalPage].dirty = false;

    // load page onto swap
    swapFile->writeToSwap(virtualPage, physicalPage);
}

int InvertedTable::getLeastUsedPage() {
    int leastUsed = this->currentVictim;
    while (true) {
        if (!this->tableEntry[leastUsed].use) {
            this->currentVictim = (leastUsed + 1) % NumPhysPages;
            return leastUsed;
        }
        this->tableEntry[leastUsed].use = false;
        leastUsed = (leastUsed + 1) % NumPhysPages;
    }
}

void InvertedTable::moveIntoTLB(TranslationEntry* tableToReplace, int physicalPage) {
    this->tableEntry[physicalPage].valid = true;
    tableToReplace->physicalPage = this->tableEntry[physicalPage].physicalPage;
    tableToReplace->virtualPage = this->tableEntry[physicalPage].virtualPage;
    tableToReplace->valid = this->tableEntry[physicalPage].valid;
}

void InvertedTable::savePageState(int physicalPage, TranslationEntry* TLBEntry) {
    this->tableEntry[physicalPage].valid = TLBEntry->valid;
    this->tableEntry[physicalPage].use = TLBEntry->use;
    this->tableEntry[physicalPage].dirty = TLBEntry->dirty;
}