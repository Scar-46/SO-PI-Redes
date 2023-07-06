#include "ProcessTable.h"

#define VECTOR_SIZE 250

ProcessTable::ProcessTable() {
    openProcess.reserve(VECTOR_SIZE);  // Reserve space for the vector
    openProcessMap = new BitMap(VECTOR_SIZE); // A bitmap to control our vector]
    this->usage = 0;  // How many threads are using this table

}

ProcessTable::~ProcessTable() {
    if (this->usage == 0) {  // If there are no threads using this table
        delete openProcessMap;  // Delete the bitmap
    }
}

int ProcessTable::addThread(Thread* childThread) {
    int threadID = openProcessMap->Find();  // Find a free place in the vector
    if (threadID == -1) {
        return -1;  // No free place
    }
    openProcess[threadID] = childThread;  // Store the Unix file handle
    this->usage++;  // Increase the usage
    return threadID;  // Return the Nachos file handle
}

int ProcessTable::delThread(int threadID) {
    if (isOpened(threadID)) {  // If the file is opened
        openProcessMap->Clear(threadID);  // Clear the bitmap entry
        this->usage--;  // Decrease the usage
        return 1;  // Return 1
    }
    return -1;  // Return -1
}

bool ProcessTable::isOpened(int threadID) {
    return openProcessMap->Test(threadID);  // Return the bitmap entry
}

Thread* ProcessTable::getchildThread(int threadID) {
    if (isOpened(threadID)) {  // If the file is opened
        return openProcess[threadID];  // Return the Unix file handle
    }
    return nullptr;  // Return nullptr
}

int ProcessTable::getUsage() {
    return this->usage;  // Return the usage
}
