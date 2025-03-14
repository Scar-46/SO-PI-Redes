// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i, size;
	this->currentTLB = 0;
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

	#ifndef VM
    	ASSERT(numPages <= (unsigned) MyMap->NumClear());		// check we're not trying
	#endif
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
	#ifdef VM
	  pageTable[i].virtualPage = -1;	// for now, virtual page # = phys page #
	  pageTable[i].physicalPage = -1;
      pageTable[i].valid = false;
   #else
  	  pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
   	  pageTable[i].physicalPage = MyMap->Find(); // find a free physical page
      pageTable[i].valid = true;
   #endif
	pageTable[i].use = false;
	pageTable[i].dirty = false;
	pageTable[i].readOnly = false;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    }

	//write the code segment into the memory
	#ifndef VM
		int codeSize = noffH.code.size;
		
		while(codeSize > 0){
			int vpn = (noffH.code.virtualAddr + noffH.code.size - codeSize) / PageSize;
			int offset = (noffH.code.virtualAddr + noffH.code.size - codeSize) % PageSize;
			int paddr = pageTable[vpn].physicalPage * PageSize + offset;
			int readSize = PageSize - offset;
			if(readSize > codeSize) readSize = codeSize;
			executable->ReadAt(&(machine->mainMemory[paddr]), readSize, noffH.code.inFileAddr + noffH.code.size - codeSize);
			codeSize -= readSize;
		}

		//write the init data segment into the memory
		int initDataSize = noffH.initData.size;
		
		while(initDataSize > 0){
			int vpn = (noffH.initData.virtualAddr + noffH.initData.size - initDataSize) / PageSize;
			int offset = (noffH.initData.virtualAddr + noffH.initData.size - initDataSize) % PageSize;
			int paddr = pageTable[vpn].physicalPage * PageSize + offset;
			int readSize = PageSize - offset;
			if(readSize > initDataSize) readSize = initDataSize;
			executable->ReadAt(&(machine->mainMemory[paddr]), readSize, noffH.initData.inFileAddr + noffH.initData.size - initDataSize);
			initDataSize -= readSize;
		}
	#endif
		
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space from an existing one.
//----------------------------------------------------------------------

AddrSpace::AddrSpace (AddrSpace* fatherSpace) {
	int stackSize = divRoundUp (UserStackSize, PageSize);
	this->filename = fatherSpace->filename;
	this->currentTLB = 0;
	numPages = fatherSpace->numPages;
    #ifndef VM
	    ASSERT (numPages <= (unsigned int) MyMap->NumClear()); // Check if there is enough space for the new process
    #endif
	pageTable = new TranslationEntry[numPages];
	for (unsigned int i = 0; i < numPages; ++i) {
		pageTable[i].virtualPage = i;
        pageTable[i].valid = true;
		#ifdef VM
			pageTable[i].virtualPage = fatherSpace->pageTable[i].virtualPage;
        #endif
		if (i < (numPages - stackSize) ) {  // if it is not the stack
			pageTable[i].physicalPage = fatherSpace->pageTable[i].physicalPage; // father and child share the same physical page
		}
		else {
			#ifndef VM
				pageTable[i].physicalPage = MyMap->Find(); // find a free physical page
				bzero (& (machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize);
			#else
				pageTable[i].virtualPage = -1;
				pageTable[i].physicalPage = -1;
				pageTable[i].valid = false;
			#endif

		}
		pageTable[i].use = false;
		pageTable[i].dirty = false;
		pageTable[i].readOnly = false;
	}
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	#ifndef VM
		for (unsigned int i = 0; i < numPages; ++i) {
			MyMap->Clear(pageTable[i].physicalPage);
		}
	#endif
	delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{
	#ifdef VM
			for (int i = 0; i < TLBSize; i++) {
			machine->tlb[i].valid = false;
		}
	#endif
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    #ifndef VM
        machine->pageTable = pageTable;
        machine->pageTableSize = numPages;
    #else
		for (int i = 0; i < TLBSize; i++) {
			machine->tlb[i].valid = false;
		}
		swapFile->resetSwap();
    #endif
}

void AddrSpace::SetFilename(const char* filename){
	this->filename = filename;
}

#ifdef VM
int AddrSpace::LoadFromMem(int virtualPage) {
	// search for a free physical page
    int physicalPage = inverMap->requestPage(virtualPage, currentThread->threadID);

    // read from executable file
    OpenFile* executable = fileSystem->Open(this->filename);
    executable->ReadAt(&(machine->mainMemory[physicalPage * PageSize]), PageSize, sizeof(NoffHeader) + virtualPage * PageSize);
    delete executable;

    return physicalPage;
}

int AddrSpace::LoadFromSwap(int virtualPage) {
    int physicalPage = inverMap->requestPage(virtualPage, currentThread->threadID); // search for a free physical page
    swapFile->readFromSwap(virtualPage, physicalPage); // read from swap
 
    // set page table
    this->pageTable[virtualPage].physicalPage = physicalPage;
    this->pageTable[virtualPage].virtualPage = virtualPage;
    this->pageTable[virtualPage].valid = true;

    return physicalPage;
}
#endif
