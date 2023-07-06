#include "NachosOpenFilesTable.h"

#define VECTOR_SIZE 250

NachosOpenFilesTable::NachosOpenFilesTable() {
    openFiles = new int[VECTOR_SIZE];  // A vector with user opened files
    openFiles[0] = 0;  // stdin
    openFiles[1] = 1;  // stdout
    openFiles[2] = 2;  // stderr

    openFilesMap = new BitMap(VECTOR_SIZE); // A bitmap to control our vector]
    openFilesMap->Mark(0);  // stdin
    openFilesMap->Mark(1);  // stdout
    openFilesMap->Mark(2);  // stderr
    this->usage = 0;  // How many threads are using this table

}

NachosOpenFilesTable::~NachosOpenFilesTable() {
    if (this->usage == 0) {  // If there are no threads using this table
        delete openFiles;  // Delete the vector
        delete openFilesMap;  // Delete the bitmap
    }
}

int NachosOpenFilesTable::Open(int UnixHandle) {
    int NachosHandle = openFilesMap->Find();  // Find a free place in the vector
    if (NachosHandle == -1) {
        return -1;  // No free place
    }
    openFiles[NachosHandle] = UnixHandle;  // Store the Unix file handle
    return NachosHandle;  // Return the Nachos file handle
}

int NachosOpenFilesTable::Close(int NachosHandle) {
    if (isOpened(NachosHandle)) {  // If the file is opened
        openFilesMap->Clear(NachosHandle);  // Clear the bitmap entry
        return 1;  // Return 1
    }
    return -1;  // Return -1
}

bool NachosOpenFilesTable::isOpened(int NachosHandle) {
    return openFilesMap->Test(NachosHandle);  // Return the bitmap entry
}

int NachosOpenFilesTable::getUnixHandle(int NachosHandle) {
    if (isOpened(NachosHandle)) {  // If the file is opened
        return openFiles[NachosHandle];  // Return the Unix file handle
    }
    return -1;  // Return -1
}

void NachosOpenFilesTable::addThread() {
    this->usage++;  // Add 1 to the usage
}

void NachosOpenFilesTable::delThread() {
    this->usage--;  // Subtract 1 to the usage
}

int NachosOpenFilesTable::getUsage() {
    return this->usage;  // Return the usage
}

void NachosOpenFilesTable::Print() {
    printf("NachosOpenFilesTable contents:\n");
    printf("Usage: %d\n", this->usage);
    printf("Vector:\n");
    for (int i = 0; i < VECTOR_SIZE; i++) {
        if (isOpened(i)) {
            printf("Vector[%d] = %d\n", i, openFiles[i]);
        }
    }
}