#include "invertedpagetable.h"

InvertedPageTable::InvertedPageTable() {
    tableMap = new BitMap(NumPhysPages);
    table = new IPTEntry[NumPhysPages];
    swapFile = new SwapFile();
    nextVictim = 0;
}

InvertedPageTable::~InvertedPageTable() {
    delete tableMap;
    delete table;
    delete swapFile;
}

int InvertedPageTable::searchVictim() {
    // Searches for a victim
    int victim = nextVictim;
    do {
        if (table[victim].entry->use) {
            table[victim].entry->use = false;
        } else {
            printf("Victim found: %d\n", victim);
            nextVictim = (victim + 1) % NumPhysPages;
            return victim;
        }
        victim = (victim + 1) % NumPhysPages;
    } while (victim != nextVictim);
    return -1;
}


int InvertedPageTable::allocateFrame(TranslationEntry * entry, int threadID) {
    int frame = tableMap->Find();
    if (frame == -1) {
        printf("No free frames\n");
        frame = searchVictim();
        deallocateFrame(frame);
        tableMap->Mark(frame);
    }
    table[frame].threadID = threadID;
    table[frame].entry = entry;
    table[frame].entry->physicalPage = frame;
    table[frame].entry->valid = true;

    return frame;
}

void InvertedPageTable::deallocateFrame(int frame) {
    tableMap->Clear(frame);
    printf("Deallocating frame %d\n", frame);
    table[frame].entry->valid = false;
    if (table[frame].entry->dirty) {
        // Write to swap file
        printf("swap out\n");
        swapFile->writeToSwapSpace(table[frame].entry->virtualPage, frame, table[frame].threadID);
    }
}

int InvertedPageTable::getFrame(TranslationEntry * entry, int threadID) {
    for (int i = 0; i < NumPhysPages; i++) {
        if (table[i].entry == entry && table[i].threadID == threadID) {
            return i;
        }
    }
    return -1;
}

int InvertedPageTable::checkSwapFile(int vpn, int threadID) {
    return swapFile->inSwapSpace(vpn, threadID);
}

void InvertedPageTable::swapIn(int vpn, int ppn, int threadID) {
    swapFile->readFromSwapSpace(vpn, ppn, threadID);
}