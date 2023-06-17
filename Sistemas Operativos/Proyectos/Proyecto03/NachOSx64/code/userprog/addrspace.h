// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
    AddrSpace (AddrSpace* fatherSpace);
					// initializing it with the program
					// stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

    void SetFilename(const char* filename);	// Set the name of the executable file
    int LoadFromMem(int virtualPage);
    int LoadFromSwap(int virtualPage);
    int GetPhysicalPage(int virtualPage);
    void MoveIntoTLB(TranslationEntry* tableToReplace, int virtTablePos);
    void SavePageState(int virtualPageState, TranslationEntry* tlbToSave);
    void Swap(int physicalPage, int virtualPage);

    int currentTLB;
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
    const char* filename;			// Name of the executable file
};

#endif // ADDRSPACE_H
