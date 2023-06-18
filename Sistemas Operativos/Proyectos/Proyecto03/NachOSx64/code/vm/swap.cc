#include "swap.h"

SwapFile::SwapFile() {
    // Create the swap file
    fileSystem->Create(SWAP_FILE_NAME, NUMBER_OF_PAGES * PageSize);
    // Open the swap file
    this->swapFile = fileSystem->Open(SWAP_FILE_NAME);
    // Create the bitmap
    this->swapMap = new BitMap(NUMBER_OF_PAGES);
}

SwapFile::~SwapFile() {
    delete this->swapFile;
}

void SwapFile::writeToSwap(int virtPage, int physPage) {
    swapFile->WriteAt(&(machine->mainMemory[physPage * PageSize]), PageSize, virtPage * PageSize);
    swapMap->Mark(virtPage);
}

void SwapFile::readFromSwap(int virtPage, int physPage) {
    swapFile->ReadAt(&(machine->mainMemory[physPage * PageSize]), PageSize, virtPage * PageSize);
    printf("Read from swap: vpn: %d ppn:%d\n", virtPage, physPage);
    swapMap->Clear(virtPage);
}

bool SwapFile::inSwap(int vpn) {
    return swapMap->Test(vpn);
}

void SwapFile::resetSwap() {
    for (int i = 0; i < NUMBER_OF_PAGES; i++) {
        swapMap->Clear(i);
    }
    delete this->swapFile;
    fileSystem->Create(SWAP_FILE_NAME, NUMBER_OF_PAGES * PageSize);
    this->swapFile = fileSystem->Open(SWAP_FILE_NAME);
}
