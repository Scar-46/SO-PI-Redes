#include "invertedpagetable.h"

InvertedPageTable::InvertedPageTable() {
    tableMap = new BitMap(NumPhysPages);
    nextVictim = 0;
}

InvertedPageTable::~InvertedPageTable() {
    delete tableMap;
}

void InvertedPageTable::addEntry(TranslationEntry * entry) {
    int pageFrame = tableMap->Find(); // Find a free place in the map
    if (pageFrame != -1) {
        int victim = searchVictim(); // Search for a victim
        if (victim != -1) {
            // If there is a victim, remove it from the table
            printf("Victim: %d\n", victim);
            removeEntry(victim);
            pageFrame = victim;
        }
    }
    entry->physicalPage = pageFrame;  // Sets the physical page
    entry->valid = true;  // Sets the entry as valid
    this->table[pageFrame] = entry; // Stores the entry
}

void InvertedPageTable::removeEntry(int pageFrame) {
    if (tableMap[pageFrame].dirty) {
        // If the entry is dirty, write it back to the disk
        int swapIndex = swapMap->Find();
        if (swapIndex == -1) {
            // No free space in the swap file
            printf("No free space in the swap file\n");
            return;
        }
        swapFile->WriteAt(&(machine->mainMemory[pageFrame * PageSize]), PageSize, swapIndex * PageSize);
    }
    tableMap->Clear(pageFrame);
}

int InvertedPageTable::searchVictim() {
    // Searches for a victim
    int victim = nextVictim;
    do {
        if (table[victim]->use) {
            table[victim]->use = false;
        } else {
            return victim;
        }
        victim = (victim + 1) % NumPhysPages;
    } while (victim != nextVictim);
    return -1;
}

TranslationEntry * InvertedPageTable::getEntry(int pageFrame) {
    if (tableMap->Test(pageFrame)) {
        return table[pageFrame];
    } else {
        return nullptr;
    }
}

void InvertedPageTable::updateTLB(TranslationEntry * entry) {
    int i;
    for (i = 0; i < TLBSize; i++) {
        if (!machine->tlb[i].valid) {
            break;
        }
    }
    if (i == TLBSize) {
        i = Random() % TLBSize;
    }
    if (machine->tlb[i].dirty) {
        // If the entry is dirty, write it back to the disk
        int swapIndex = swapMap->Find();
        if (swapIndex == -1) {
            // No free space in the swap file
            printf("No free space in the swap file\n");
            return;
        }
        swapFile->WriteAt(&(machine->mainMemory[machine ->tlb[i].physicalPage * PageSize]), PageSize, swapIndex * PageSize);
    }
    machine->tlb[i] = *entry;
}


void InvertedPageTable::getFromSwap(int pageFrame) {
    // Gets an entry from the swap file
    if(!swapMap->Test(pageFrame)) {
        return nullptr;
    } else {
        swapFile->ReadAt(&(machine->mainMemory[pageFrame * PageSize]), PageSize, pageFrame * PageSize); //TODO(): Repair the swap index.
        swapMap->Clear(swapIndex);
    }
}

void InvertedPageTable::getFromExecutable(int pageFrame) {
    // Gets an entry from the executable file
    bzero(&(machine->mainMemory[pageFrame * PageSize]), PageSize);
    executable->ReadAt(&(machine->mainMemory[pageFrame * PageSize]), PageSize, noffH.code.inFileAddr + pageFrame * PageSize); //TODO(): Repair the executable index.
}


