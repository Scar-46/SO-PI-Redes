// NachosOpenFilesTable.cc 
//	Handles the table that keeps track of open Nachos files
//
// Copyright (c) 2023 Oscar Mario Fernández Jiménez
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <stdio.h>
#include "bitmap.h"

#ifndef NACHOS_OPEN_FILES_TABLE

class NachosOpenFilesTable {
  public:
    NachosOpenFilesTable();						// Initialize
    ~NachosOpenFilesTable();						// De-allocate
    
    int Open( int UnixHandle );					// Register the file handle
    int Close( int NachosHandle );					// Unregister the file handle
    bool isOpened( int NachosHandle );
    int getUnixHandle( int NachosHandle );
    void addThread();							// If a user thread is using this table, add it
    void delThread();							// If a user thread is using this table, delete it
    int getUsage();								// Return the number of threads using this table
    
    void Print();								// Print contents
    
  private:
    int * openFiles;							// A vector with user opened files
    BitMap * openFilesMap;						// A bitmap to control our vector

    int usage;									// How many threads are using this table

};

#endif // NACHOS_OPEN_FILES_TABLE
