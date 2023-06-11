#include "swap.h"

SwapFile::SwapFile() {
    for (int i = 0; i < NUMBER_OF_PAGES; i++) {
        swapMap[i] = false;
    }
  	fileSystem->Create(SWAP_FILE_NAME, NUMBER_OF_PAGES * PageSize);
	swapFile = fileSystem->Open(SWAP_FILE_NAME);
}

SwapFile::~SwapFile() {
  delete swapFile;
  fileSystem->Remove(SWAP_FILE_NAME);
}

void SwapFile::writeToSwapSpace(int vpn, int ppn, int threadID) {
    char* from = machine->mainMemory + ppn * PageSize;
    int into = vpn * PageSize;

    swapFile->WriteAt(from, PageSize, into);
    swapMap[vpn] = true;
    threadMap[vpn] = threadID;
}

void SwapFile::readFromSwapSpace(int vpn, int ppn, int threadID) {
    char* into = machine->mainMemory + ppn * PageSize;
    int from = vpn * PageSize;

    swapFile->ReadAt(into, PageSize, from);
    swapMap[vpn] = false;
    threadMap[vpn] = -1;
}

bool SwapFile::inSwapSpace(int vpn, int threadID) {
    return swapMap[vpn] && threadMap[vpn] == threadID;
}