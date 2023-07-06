// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <fcntl.h> // for open system call
#include <unistd.h> // for read system call
#include <sys/socket.h> // for socket system call
#include <string.h>  // for memset
#include <arpa/inet.h>  // for inet_pton
#include <sys/types.h>  // for connect

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synch.h"
#include "bitmap.h" 


#define BUFFERSIZE 1024
#define ERROR -1

Semaphore *console = new Semaphore("console", 1);

void returnFromSystemCall() {

   machine->WriteRegister( PrevPCReg, machine->ReadRegister( PCReg ) );		// PrevPC <- PC
   machine->WriteRegister( PCReg, machine->ReadRegister( NextPCReg ) );			// PC <- NextPC
   machine->WriteRegister( NextPCReg, machine->ReadRegister( NextPCReg ) + 4 );	// NextPC <- NextPC + 4

}  // returnFromSystemCall

void reading (char* buffer, int reg) {
   int addr = machine->ReadRegister(reg);
   int i = 0;
   do {
      if(!machine->ReadMem(addr + i, 1, (int*)&buffer[i])) {
         machine->ReadMem(addr + i, 1, (int*)&buffer[i]); // first read may fail if address is not mapped yet
      }
   } while(buffer[i++] != '\0');

} // reading 

/*
 *  System call interface: Halt()
 */
void NachOS_Halt() {		// System call 0
	DEBUG('a', "Shutdown, initiated by user program.\n");
   printf("\n");
   currentThread->Finish();
   interrupt->Halt();
}


/*
 *  System call interface: void Exit( int )
 */
void NachOS_Exit() {		// System call 1
   DEBUG('a', "Exit, initiated by user program.\n");
   int status = machine->ReadRegister(4);
   if (currentThread->openFilesTable->getUsage() == 0) {
      NachOS_Halt();
   }
   currentThread->openFilesTable->delThread();
   if (currentThread->space != NULL) {
      delete currentThread->space;
   }
   if (currentThread->joinSem != nullptr) {
      currentThread->joinSem->V();
   }
   currentThread->Finish();
   machine->WriteRegister(2, status);
   returnFromSystemCall();
   DEBUG('a', "Exiting exit.\n");
   
} // NachOS_Exit


void execAux (void* filename) {
   DEBUG('a', "Exec aux, initiated by user program.\n");
	char* filenameAux = (char*) filename;
   OpenFile *executable = fileSystem->Open(filenameAux);
	if (executable == NULL) {
		printf ("Unable to open file %s\n", filenameAux);
      delete executable;
		return;
	}
	currentThread->space = new AddrSpace (executable); // create a new address space
   currentThread->space->SetFilename((char*)filenameAux); // set the filename
   delete executable;			// close file
	currentThread->space->InitRegisters(); // set the initial register values
	currentThread->space->RestoreState();  // load page table register
   DEBUG('a', "Finishing Exec aux.\n");
	machine->Run(); // jump to the user progam
   ASSERT(false); // machine->Run never returns;
}


/*
 *  System call interface: SpaceId Exec( char * )
 */
void NachOS_Exec() {		// System call 2
   DEBUG('a', "Exec, initiated by user program.\n");
   char* buffer = new char[BUFFERSIZE];
   int threadID;
   reading (buffer, 4);
   Thread *thread = new Thread("Exec Thread");
   threadID = currentThread->processTable->addThread(thread);
   thread->Fork(execAux, (void *) buffer);
   machine->WriteRegister(2, threadID); // Return the thread ID
   returnFromSystemCall();
   DEBUG('a', "Finishing Exec.\n");
} // NachOS_Exec


/*
 *  System call interface: int Join( SpaceId )
 */
void NachOS_Join() {		// System call 3
   DEBUG('a', "Join, initiated by user program.\n");
   int idThread = machine->ReadRegister(4);
   Thread *thread = (Thread *) currentThread->processTable->getchildThread(idThread);
   if (thread == NULL) {
      printf("Unable to find thread %d\n", idThread);
      machine->WriteRegister(2, ERROR); // Return -1 if error
   } else {
      Semaphore *sem = new Semaphore("sem", 0);
      thread->joinSem = sem;
      sem->P();
      currentThread->processTable->delThread(idThread);
   }
   returnFromSystemCall();
   DEBUG('a', "Finishing Join.\n");
}


/*
 *  System call interface: void Create( char * )
 */
void NachOS_Create() {		// System call 4
   DEBUG('a', "Create file, initiated by user program.\n");
   char buffer[BUFFERSIZE];
   reading (buffer, 4);
   int idFileUnix = creat(buffer, 0666); // 0666: Read and write permission for owner, group, and others
   if (idFileUnix == ERROR) {
      printf("Unable to create file %s\n", buffer);
      machine->WriteRegister(2, ERROR); // Return -1 if error
   } else {
      currentThread->openFilesTable->addThread();
      int idFileNachos = currentThread->openFilesTable->Open(idFileUnix);
      machine->WriteRegister(2, idFileNachos); // Return the file descriptor
   }
   returnFromSystemCall();
   DEBUG('a', "Finishing Create.\n");
}


/*
 *  System call interface: OpenFileId Open( char * )
 */
void NachOS_Open() {		// System call 5
   DEBUG('a', "Open file, initiated by user program.\n");
   char buffer[BUFFERSIZE];
   reading (buffer, 4);
   int idFileUnix = open(buffer, O_RDWR); // O_RDWR: Open for reading and writing. The file is created if it does not exist.
   if (idFileUnix == ERROR) {
      printf("Unable to open file %s\n", buffer);
      machine->WriteRegister(2, ERROR); // Return -1 if error
   } else {
      int idFileNachos = currentThread->openFilesTable->Open(idFileUnix);
      machine->WriteRegister(2, idFileNachos); // Return the file descriptor
   }
   DEBUG('a', "Finishing Open.\n");
   returnFromSystemCall();
}  // NachOS_Open


/*
 *  System call interface: OpenFileId Write( char *, int, OpenFileId )
 */
void NachOS_Write() {		// System call 6
   DEBUG('a', "Write to file, initiated by user program.\n");
   char buffer[BUFFERSIZE];
   int size = machine->ReadRegister( 5 );	// Read size to write
   // buffer = Read data from address given by user;
   reading (buffer, 4);
   OpenFileId descriptor = machine->ReadRegister( 6 );	// Read file descriptor
	// Need a semaphore to synchronize access to console
	console->P();
	switch ( descriptor ) {
		case  ConsoleInput:	// User could not write to standard input
			machine->WriteRegister( 2, ERROR);
			break;
		case  ConsoleOutput:
		   buffer[ size ] = 0;
         printf( "%s", buffer );
         fflush( stdout );
         stats->numConsoleCharsWritten += size;
		break;
		case ConsoleError:	// This trick permits to write integers to console
         printf( "%d\n", machine->ReadRegister( 4 ) );
			break;
		default:	// All other opened files
         if (currentThread->openFilesTable->isOpened(descriptor)) {
            int idFileUnix = currentThread->openFilesTable->getUnixHandle(descriptor);
            int numWrite = write(idFileUnix, buffer, size);
            machine->WriteRegister(2, numWrite);
			   // Return the number of chars written to user, via r2
            stats->numDiskWrites++;
         } else {
            printf("Unable to write to file %d\n", descriptor);
            machine->WriteRegister(2, ERROR);
         }
			break;

	} // switch
	console->V();
   DEBUG('a', "Finishing Write.\n");
   returnFromSystemCall();		// Update the PC registers
}  // NachOS_Write

/*
 *  System call interface: OpenFileId Read( char *, int, OpenFileId )
 */
void NachOS_Read() {		// System call 7
   DEBUG('a', "Read from file, initiated by user program.\n");
   char buffer[BUFFERSIZE];
   int size = machine->ReadRegister( 5 );	// Read size to read
   int numRead = 0;  // Number of chars read

   // buffer = Read data from address given by user;

   OpenFileId descriptor = machine->ReadRegister( 6 );	// Read file descriptor
   // Need a semaphore to synchronize access to console
   console->P();
   switch ( descriptor ) {
      case  ConsoleInput:	// User can read from standard input
         numRead = read(0, buffer, size);
         machine->WriteRegister( 2, numRead );
          for (int i = 0; i < numRead; ++i) {
            if (!machine->WriteMem(machine->ReadRegister(4) + i, 1, buffer[i])) {
               machine->WriteMem(machine->ReadRegister(4) + i, 1, buffer[i]);
            }
         }
         stats->numConsoleCharsRead += numRead;
         break;
      case  ConsoleOutput: // User could not read from standard output
         printf( "ERROR: ConsoleOutput\n");
         machine->WriteRegister( 2, ERROR);
      break;
      case ConsoleError: 
         printf( "ERROR: ConsoleError\n");
         break;
      default:	// All other read files
         if (currentThread->openFilesTable->isOpened(descriptor)) {
            int idFileUnix = currentThread->openFilesTable->getUnixHandle(descriptor);
            numRead = read(idFileUnix, buffer, size);
            machine->WriteRegister(2, numRead);
            for (int i = 0; i < numRead; ++i) {
               if (!machine->WriteMem(machine->ReadRegister(4) + i, 1, buffer[i])) {
                  machine->WriteMem(machine->ReadRegister(4) + i, 1, buffer[i]);
               }
            }
            stats->numDiskReads++;
         } else {
            machine->WriteRegister(2, ERROR);
            printf("Unable to read file %d\n", descriptor);
         }
         break;
   } // switch
   console->V();
   DEBUG('a', "Exit Read\n");
   returnFromSystemCall();		// Update the PC registers
}  // NachOS_Read


/*
 *  System call interface: void Close( OpenFileId )
 */
void NachOS_Close() {		// System call 8
   DEBUG('a', "Close file, initiated by user program.\n");
   OpenFileId idFileNachos = machine->ReadRegister(4);
   if (currentThread->openFilesTable->isOpened(idFileNachos)) {
      int idFileUnix = currentThread->openFilesTable->getUnixHandle(idFileNachos);
      currentThread->openFilesTable->Close(idFileNachos);
      close(idFileUnix);
   } else {
      printf("Unable to close file %d\n", idFileNachos);
   }
   returnFromSystemCall();
   DEBUG('a', "Finishing Close.\n");
}

void NachosForkThread( void * p ) { // for 64 bits version

   AddrSpace *space;
   space = currentThread->space;
   space->InitRegisters();             // set the initial register values
   space->RestoreState();              // load page table register

   // Set the return address for this thread to the same as the main thread
   // This will lead this thread to call the exit system call and finish
   machine->WriteRegister( RetAddrReg, 4 );
   machine->WriteRegister( PCReg, (long) p );
   machine->WriteRegister( NextPCReg, (long) p + 4 );

   machine->Run();                     // jump to the user progam
} // NachosForkThread

/*
 *  System call interface: void Fork( void (*func)() )
 */
void NachOS_Fork() {		// System call 9
   DEBUG( 'u', "Entering Fork System call\n" );
   // We need to create a new kernel thread to execute the user thread
	Thread * newT = new Thread( "child to execute Fork code" );
   // We need to share the Open File Table structure with this new child
   newT->openFilesTable = currentThread->openFilesTable;
   newT->threadID = currentThread->threadID; // TODO(): Unmark the thread ID map
	// Child and father will also share the same address space, except for the stack
	// Text, init data and uninit data are shared, a new stack area must be created
	// for the new child
   currentThread->openFilesTable->addThread();
	// We suggest the use of a new constructor in AddrSpace class,
	// This new constructor will copy the shared segments (space variable) from currentThread, passed
	// as a parameter, and create a new stack for the new child
   newT->space = new AddrSpace( currentThread->space );
   // We (kernel)-Fork to a new method to execute the child code
	// Pass the user routine address, now in register 4, as a parameter
   newT->Fork( NachosForkThread, (void *) machine->ReadRegister( 4 ) );

	returnFromSystemCall();	// This adjust the PrevPC, PC, and NextPC registers
   DEBUG( 'u', "Exiting Fork System call\n" );
}  // NachOS_Fork

/*
 *  System call interface: void Yield()
 */
void NachOS_Yield() {		// System call 10
   DEBUG( 'u', "Entering Yield System call\n" );
   currentThread->Yield();
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Yield System call\n" );
} // NachOS_Yield


/*
 *  System call interface: Sem_t SemCreate( int )
 */
void NachOS_SemCreate() {		// System call 11
   DEBUG( 'u', "Entering SemCreate System call\n" );
   int initValue = machine->ReadRegister( 4 );
   Semaphore * newSem = new Semaphore( "new semaphore", initValue );
   int semId = semsBitMap->Find();
   if ( semId != ERROR ) {
      semArray[ semId ] = newSem;
   }
   machine->WriteRegister( 2, semId );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting SemCreate System call\n" );
}


/*
 *  System call interface: int SemDestroy( Sem_t )
 */
void NachOS_SemDestroy() {		// System call 12
   DEBUG( 'u', "Entering SemDestroy System call\n" );
   int semId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( semsBitMap->Test( semId ) ) {
      status = 0;
      semsBitMap->Clear( semId );
      delete semArray[ semId ];
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting SemDestroy System call\n" );
}


/*
 *  System call interface: int SemSignal( Sem_t )
 */
void NachOS_SemSignal() {		// System call 13
   DEBUG( 'u', "Entering SemSignal System call\n" );
   int semId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( semsBitMap->Test( semId ) ) {
      status = 0;
      semArray[ semId ]->V();
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting SemSignal System call\n" );
}


/*
 *  System call interface: int SemWait( Sem_t )
 */
void NachOS_SemWait() {		// System call 14
   DEBUG( 'u', "Entering SemWait System call\n" );
   int semId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( semsBitMap->Test( semId ) ) {
      status = 0;
      semArray[ semId ]->P();
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting SemWait System call\n" );
}


/*
 *  System call interface: Lock_t LockCreate( int )
 */
void NachOS_LockCreate() {		// System call 15
   DEBUG( 'u', "Entering LockCreate System call\n" );
   Lock * newLock = new Lock( "new lock" );
   int lockId = locksBitMap->Find();
   if ( lockId != ERROR ) {
      lockArray[ lockId ] = newLock;
   }
   machine->WriteRegister( 2, lockId );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting LockCreate System call\n" );
}


/*
 *  System call interface: int LockDestroy( Lock_t )
 */
void NachOS_LockDestroy() {		// System call 16
   DEBUG( 'u', "Entering LockDestroy System call\n" );
   int lockId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( locksBitMap->Test( lockId ) ) {
      status = 0;
      locksBitMap->Clear( lockId );
      delete lockArray[ lockId ];
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting LockDestroy System call\n" );
}


/*
 *  System call interface: int LockAcquire( Lock_t )
 */
void NachOS_LockAcquire() {		// System call 17
   DEBUG( 'u', "Entering LockAcquire System call\n" );
   int lockId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( locksBitMap->Test( lockId ) ) {
      status = 0;
      lockArray[ lockId ]->Acquire();
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting LockAcquire System call\n" );
}


/*
 *  System call interface: int LockRelease( Lock_t )
 */
void NachOS_LockRelease() {		// System call 18
   DEBUG( 'u', "Entering LockRelease System call\n" );
   int lockId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( locksBitMap->Test( lockId ) ) {
      status = 0;
      lockArray[ lockId ]->Release();
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting LockRelease System call\n" );
}


/*
 *  System call interface: Cond_t LockCreate( int )
 */
void NachOS_CondCreate() {		// System call 19
   DEBUG( 'u', "Entering CondCreate System call\n" );
   Condition * newCond = new Condition( "new condition");
   int condId = condsBitMap->Find();
   if ( condId != ERROR ) {
      condArray[ condId ] = newCond;
   }
   machine->WriteRegister( 2, condId );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting CondCreate System call\n" );
}


/*
 *  System call interface: int CondDestroy( Cond_t )
 */
void NachOS_CondDestroy() {		// System call 20
   DEBUG( 'u', "Entering CondDestroy System call\n" );
   int condId = machine->ReadRegister( 4 );
   int status = ERROR;
   if ( condsBitMap->Test( condId ) ) {
      status = 0;
      condsBitMap->Clear( condId );
      delete condArray[ condId ];
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting CondDestroy System call\n" );
}


/*
 *  System call interface: int CondSignal( Cond_t )
 */
void NachOS_CondSignal() {		// System call 21
   DEBUG( 'u', "Entering CondSignal System call\n" );
   int condId = machine->ReadRegister( 4 );
   int lockId = machine->ReadRegister( 5 );
   int status = ERROR;
   if ( condsBitMap->Test( condId ) ) {
      status = 0;
      condArray[ condId ]->Signal( lockArray[ lockId ] );
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
}


/*
 *  System call interface: int CondWait( Cond_t )
 */
void NachOS_CondWait() {		// System call 22
   DEBUG( 'u', "Entering CondWait System call\n" );
   int condId = machine->ReadRegister( 4 );
   int lockId = machine->ReadRegister( 5 );
   int status = ERROR;
   if ( condsBitMap->Test( condId ) ) {
      status = 0;
      condArray[ condId ]->Wait( lockArray[ lockId ] );
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting CondWait System call\n" );
}


/*
 *  System call interface: int CondBroadcast( Cond_t )
 */
void NachOS_CondBroadcast() {		// System call 23
   DEBUG( 'u', "Entering CondBroadcast System call\n" );
   int condId = machine->ReadRegister( 4 );
   int lockId = machine->ReadRegister( 5 );
   int status = ERROR;
   if ( condsBitMap->Test( condId ) ) {
      status = 0;
      condArray[ condId ]->Broadcast( lockArray[ lockId ] );
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting CondBroadcast System call\n" );
}


/*
 *  System call interface: Socket_t Socket( int, int )
 */
void NachOS_Socket() {			// System call 30
   DEBUG( 'u', "Entering Socket System call\n" );
   int family = AF_INET;
   if ( machine->ReadRegister( 4 ) == 1 ) {
      family = AF_INET6;
   }
   int type = SOCK_STREAM;
   if ( machine->ReadRegister( 5 ) == 1 ) {
      type = SOCK_DGRAM;
   }
   int socketId = socket(family, type, 0);

   if (socketId == ERROR) {
      printf("Unable to create socket\n");
      machine->WriteRegister(2, ERROR); // Return -1 if error
   } else {
      int idFileNachos = currentThread->openFilesTable->Open(socketId);
      machine->WriteRegister(2, idFileNachos); // Return the file descriptor
   }
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Socket System call\n" );
}


/*
 *  System call interface: Socket_t Connect( char *, int )
 */
void NachOS_Connect() {		// System call 31
   DEBUG( 'u', "Entering Connect System call\n" );
   int idFileNachos = machine->ReadRegister( 4 );
   int socketId = currentThread->openFilesTable->getUnixHandle(idFileNachos);
   struct sockaddr_in hostAddr;
   struct sockaddr * ha;
   memset(&hostAddr, 0, sizeof(hostAddr));
   hostAddr.sin_family = AF_INET;
   hostAddr.sin_port = htons( machine->ReadRegister( 6 ) );
   char hostName[BUFFERSIZE];
   reading (hostName, 5);
   inet_pton(AF_INET, hostName, &hostAddr.sin_addr);
   ha = (struct sockaddr *) &hostAddr;
   int status = connect(socketId, ha, sizeof(hostAddr));
   if (status == ERROR) {
      printf("Unable to connect\n");
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Connect System call\n" );
}


/*
 *  System call interface: int Bind( Socket_t, int )
 */
void NachOS_Bind() {		// System call 32
   DEBUG( 'u', "Entering Bind System call\n" );
   int idFileNachos = machine->ReadRegister( 4 );
   int socketId = currentThread->openFilesTable->getUnixHandle(idFileNachos);
   struct sockaddr_in serverAddr;
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons( machine->ReadRegister( 5 ) );
   serverAddr.sin_addr.s_addr = INADDR_ANY;
   int status = bind(socketId, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   if (status == ERROR) {
      printf("Unable to bind socket\n");
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Bind System call\n" );
}


/*
 *  System call interface: int Listen( Socket_t, int )
 */
void NachOS_Listen() {		// System call 33
   DEBUG( 'u', "Entering Listen System call\n" );
   int idFileNachos = machine->ReadRegister( 4 );
   int socketId = currentThread->openFilesTable->getUnixHandle(idFileNachos);
   int status = listen(socketId, machine->ReadRegister( 5 ));
   if (status == ERROR) {
      printf("Unable to listen\n");
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Listen System call\n" );
}


/*
 *  System call interface: int Accept( Socket_t )
 */
void NachOS_Accept() {		// System call 34
   DEBUG( 'u', "Entering Accept System call\n" );
   int idFileNachos = machine->ReadRegister( 4 );
   int socketId = currentThread->openFilesTable->getUnixHandle(idFileNachos);
   struct sockaddr_in6 clientAddr;
   socklen_t clientAddrLen = sizeof(clientAddr);
   int status = accept(socketId, (struct sockaddr *) &clientAddr, &clientAddrLen);
   if (status == ERROR) {
      printf("Unable to accept connection\n");
      machine->WriteRegister(2, ERROR); // Return -1 if error
   } else {
      currentThread->openFilesTable->Close(idFileNachos);
      int newSocketId = currentThread->openFilesTable->Open(status);
      machine->WriteRegister(2, newSocketId); // Return the file descriptor
   }
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Accept System call\n" );
}


/*
 *  System call interface: int Shutdown( Socket_t, int )
 */
void NachOS_Shutdown() {	// System call 25
   DEBUG( 'u', "Entering Shutdown System call\n" );
   int idFileNachos = machine->ReadRegister( 4 );
   int socketId = currentThread->openFilesTable->getUnixHandle(idFileNachos);
   int status = shutdown(socketId, machine->ReadRegister( 5 ));
   if (status == ERROR) {
      printf("Unable to shutdown connection\n");
   }
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting Shutdown System call\n" );
}
#ifdef VM
TranslationEntry* secondChance() {
   TranslationEntry* entry;
   int currentTLB = currentThread->space->currentTLB;
   while(true) {
      entry = &(machine->tlb[currentTLB]);
      currentTLB = (currentTLB + 1) % TLBSize;
      if(entry->use) {
         entry->use = false;
      } else {
         currentThread->space->currentTLB = currentTLB;
         return entry;
      }
   }
}

void saveTLB() {
   for (int i = 0; i < TLBSize; i++) {
      TranslationEntry* entry = &(machine->tlb[i]);
      if (entry->valid || entry->use || entry->dirty) {
         inverMap->savePageState(entry->physicalPage, entry);
      }
   }
}

void updateTLB(int physicalPage) {
   TranslationEntry* replaceEntry = secondChance();
   // update the TLB entry
   inverMap->moveIntoTLB(replaceEntry, physicalPage);
}

void NachOS_PageFault() {
   stats->numPageFaults++; // increment page faults
   int badAddr = machine->ReadRegister(BadVAddrReg);
   int vpn = badAddr / PageSize; // virtual page number of the faulting address
   // save the state of the tlb in the inverted page table
   saveTLB();
   // check if the page is in the inverted page table
   int ppn = inverMap->findPage(vpn, currentThread->threadID);
   // if not found, check if it is in the swap file or in the executable
   if (ppn == -1) {
      if (swapFile->inSwap(vpn)) {
         // if found in swap, load from swap
         ppn = currentThread->space->LoadFromSwap(vpn);
      } else {
         // if not found, load from storage
         ppn = currentThread->space->LoadFromMem(vpn);
      }
   }
   updateTLB(ppn);
}
#endif
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch ( which ) {

       case SyscallException:
          switch ( type ) {
             case SC_Halt:		// System call # 0
                NachOS_Halt();
                break;
             case SC_Exit:		// System call # 1
                NachOS_Exit();
                break;
             case SC_Exec:		// System call # 2
                NachOS_Exec();
                break;
             case SC_Join:		// System call # 3
                NachOS_Join();
                break;

             case SC_Create:		// System call # 4
                NachOS_Create();
                break;
             case SC_Open:		// System call # 5
                NachOS_Open();
                break;
             case SC_Read:		// System call # 6
                NachOS_Read();
                break;
             case SC_Write:		// System call # 7
                NachOS_Write();
                break;
             case SC_Close:		// System call # 8
                NachOS_Close();
                break;

             case SC_Fork:		// System call # 9
                NachOS_Fork();
                break;
             case SC_Yield:		// System call # 10
                NachOS_Yield();
                break;

             case SC_SemCreate:         // System call # 11
                NachOS_SemCreate();
                break;
             case SC_SemDestroy:        // System call # 12
                NachOS_SemDestroy();
                break;
             case SC_SemSignal:         // System call # 13
                NachOS_SemSignal();
                break;
             case SC_SemWait:           // System call # 14
                NachOS_SemWait();
                break;

             case SC_LckCreate:         // System call # 15
                NachOS_LockCreate();
                break;
             case SC_LckDestroy:        // System call # 16
                NachOS_LockDestroy();
                break;
             case SC_LckAcquire:         // System call # 17
                NachOS_LockAcquire();
                break;
             case SC_LckRelease:           // System call # 18
                NachOS_LockRelease();
                break;

             case SC_CondCreate:         // System call # 19
                NachOS_CondCreate();
                break;
             case SC_CondDestroy:        // System call # 20
                NachOS_CondDestroy();
                break;
             case SC_CondSignal:         // System call # 21
                NachOS_CondSignal();
                break;
             case SC_CondWait:           // System call # 22
                NachOS_CondWait();
                break;
             case SC_CondBroadcast:           // System call # 23
                NachOS_CondBroadcast();
                break;

             case SC_Socket:	// System call # 30
		NachOS_Socket();
               break;
             case SC_Connect:	// System call # 31
		NachOS_Connect();
               break;
             case SC_Bind:	// System call # 32
		NachOS_Bind();
               break;
             case SC_Listen:	// System call # 33
		NachOS_Listen();
               break;
             case SC_Accept:	// System call # 32
		NachOS_Accept();
               break;
             case SC_Shutdown:	// System call # 33
		NachOS_Shutdown();
               break;

             default:
                printf("Unexpected syscall exception %d\n", type );
                ASSERT( false );
                break;
          }
          break;

       case PageFaultException: {
            #ifdef VM
               NachOS_PageFault();
            #endif
          break;
       }

       case ReadOnlyException:
          printf( "Read Only exception (%d)\n", which );
          ASSERT( false );
          break;

       case BusErrorException:
          printf( "Bus error exception (%d)\n", which );
          ASSERT( false );
          break;

       case AddressErrorException:
          printf( "Address error exception (%d)\n", which );
          ASSERT( false );
          break;

       case OverflowException:
          printf( "Overflow exception (%d)\n", which );
          ASSERT( false );
          break;

       case IllegalInstrException:
          printf( "Ilegal instruction exception (%d)\n", which );
          ASSERT( false );
          break;

       default:
          printf( "Unexpected exception %d\n", which );
          ASSERT( false );
          break;
    }

}
