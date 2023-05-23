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

#include <fcntl.h>
#include <unistd.h>

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synch.h"
//#include "bitmap.h"

#define BUFFERSIZE 1024
#define ERROR -1

Semaphore *console = new Semaphore("console", 1);
Semaphore * semArray[10];
//BitMap * semsBitMap = new BitMap(10);

void returnFromSystemCall() {

   machine->WriteRegister( PrevPCReg, machine->ReadRegister( PCReg ) );		// PrevPC <- PC
   machine->WriteRegister( PCReg, machine->ReadRegister( NextPCReg ) );			// PC <- NextPC
   machine->WriteRegister( NextPCReg, machine->ReadRegister( NextPCReg ) + 4 );	// NextPC <- NextPC + 4

}  // returnFromSystemCall

void reading (char* buffer) {

	int character = 1;
	for (int i = 0; character != '\0'; ++i) {
		machine->ReadMem(machine->ReadRegister(4) + i, 1, &character);
		buffer[i] = (char) character;
	}

} // reading 

/*
 *  System call interface: Halt()
 */
void NachOS_Halt() {		// System call 0

	DEBUG('a', "Shutdown, initiated by user program.\n");
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
      currentThread->space = NULL;
   }
   if (currentThread->joinSem != nullptr) {
      currentThread->joinSem->V();
   }
   currentThread->Finish();
   machine->WriteRegister(2, status);
   returnFromSystemCall();
   DEBUG('a', "Exiting exit.\n");
   
} // NachOS_Exit


void execAux (void* executableName) {
   DEBUG('a', "Exec aux, initiated by user program.\n");
	OpenFile *executable = (OpenFile *) executableName;
	if (executable == NULL) {
		printf ("Unable to open file");
		return;
	}
	currentThread->space = new AddrSpace (executable); // create a new address space
	currentThread->space->InitRegisters(); // set the initial register values
	currentThread->space->RestoreState();  // load page table register
	machine->Run(); // jump to the user progam
   ASSERT(false); // machine->Run never returns;
   DEBUG('a', "Finishing Exec aux.\n");
}


/*
 *  System call interface: SpaceId Exec( char * )
 */
void NachOS_Exec() {		// System call 2
   DEBUG('a', "Exec, initiated by user program.\n");
   char buffer[BUFFERSIZE];
   int threadID;
   reading (buffer);
   OpenFile *executable = fileSystem->Open(buffer);
   printf("Opening file %s\n", buffer);
   if (executable == NULL) {
      printf ("Unable to open file");
      machine->WriteRegister(2, ERROR); // Return -1 if error
      delete executable;
   } else {
      Thread *thread = new Thread("Exec Thread");
      threadID = currentThread->processTable->addThread(thread);
      thread->Fork(execAux, (void *) executable);
      machine->WriteRegister(2, threadID); // Return the thread ID
   }
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
   reading (buffer);
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
   reading (buffer);
   
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
   reading (buffer);
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
            bool flag;
            for (int i = 0; i < numRead; ++i && flag) {
               flag = machine->WriteMem(machine->ReadRegister(4) + i, 1, buffer[i]); //TODO: check if it is correct
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
printf("Fork\n");
   DEBUG( 'u', "Entering Fork System call\n" );
   // We need to create a new kernel thread to execute the user thread
	Thread * newT = new Thread( "child to execute Fork code" );
   // We need to share the Open File Table structure with this new child
   newT->openFilesTable = currentThread->openFilesTable;
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
   //int semId = semsBitMap->Find();
   //if ( semId != ERROR ) {
   //   semArray[ semId ] = newSem;
   //}
   //machine->WriteRegister( 2, semId );
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
   /*
   if ( semsBitMap->Test( semId ) ) {
      semsBitMap->Clear( semId );
      delete semArray[ semId ];
      semArray[ semId ] = NULL;
      status = 0;
   }
   */
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
   /*
   if ( semsBitMap->Test( semId ) ) {
      semArray[ semId ]->V();
      status = 0;
   }*/
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
   /*
   if ( semsBitMap->Test( semId ) ) {
      semArray[ semId ]->P();
      status = 0;
   }*/
   machine->WriteRegister( 2, status );
   returnFromSystemCall();
   DEBUG( 'u', "Exiting SemWait System call\n" );
}


/*
 *  System call interface: Lock_t LockCreate( int )
 */
void NachOS_LockCreate() {		// System call 15

}


/*
 *  System call interface: int LockDestroy( Lock_t )
 */
void NachOS_LockDestroy() {		// System call 16
}


/*
 *  System call interface: int LockAcquire( Lock_t )
 */
void NachOS_LockAcquire() {		// System call 17
}


/*
 *  System call interface: int LockRelease( Lock_t )
 */
void NachOS_LockRelease() {		// System call 18
}


/*
 *  System call interface: Cond_t LockCreate( int )
 */
void NachOS_CondCreate() {		// System call 19
}


/*
 *  System call interface: int CondDestroy( Cond_t )
 */
void NachOS_CondDestroy() {		// System call 20
}


/*
 *  System call interface: int CondSignal( Cond_t )
 */
void NachOS_CondSignal() {		// System call 21
}


/*
 *  System call interface: int CondWait( Cond_t )
 */
void NachOS_CondWait() {		// System call 22
}


/*
 *  System call interface: int CondBroadcast( Cond_t )
 */
void NachOS_CondBroadcast() {		// System call 23
}


/*
 *  System call interface: Socket_t Socket( int, int )
 */
void NachOS_Socket() {			// System call 30
}


/*
 *  System call interface: Socket_t Connect( char *, int )
 */
void NachOS_Connect() {		// System call 31
}


/*
 *  System call interface: int Bind( Socket_t, int )
 */
void NachOS_Bind() {		// System call 32
}


/*
 *  System call interface: int Listen( Socket_t, int )
 */
void NachOS_Listen() {		// System call 33
}


/*
 *  System call interface: int Accept( Socket_t )
 */
void NachOS_Accept() {		// System call 34
}


/*
 *  System call interface: int Shutdown( Socket_t, int )
 */
void NachOS_Shutdown() {	// System call 25
}

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
