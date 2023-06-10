
// NachosOpenFilesTable.cc 
//	Handles the table that keeps track of open Nachos files
//
// Copyright (c) 2023 Oscar Mario Fernández Jiménez
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "thread.h"
#include <stdio.h>
#include "bitmap.h"
#include <vector>

#ifndef PROCESS_TABLE
#define PROCESS_TABLE

class Thread;
class ProcessTable {
  public:
    ProcessTable();
    ~ProcessTable();

    int addThread(Thread* childThread);
    int delThread(int threadID);
    bool isOpened(int threadID);
    Thread* getchildThread(int threadID);
    int getUsage();	
    
  private:
    std::vector<Thread*> openProcess;
    BitMap * openProcessMap;
    int usage;

};

#endif // PROCESS_TABLE
