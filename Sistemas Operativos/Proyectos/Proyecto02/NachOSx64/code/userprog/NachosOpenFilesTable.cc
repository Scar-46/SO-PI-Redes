#include "NachosOpenFilesTable.h"

#define VECTOR_SIZE 250

NachosOpenFilesTable::NachosOpenFilesTable() {
    openFiles = new int[VECTOR_SIZE];  // A vector with user opened files
    openFilesMap = new BitMap(VECTOR_SIZE); // A bitmap to control our vector
    this->usage = 0;  // How many threads are using this table
}

NachosOpenFilesTable::~NachosOpenFilesTable() {
    delete openFiles;  // De-allocate openFiles
    delete openFilesMap; // De-allocate openFilesMap
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