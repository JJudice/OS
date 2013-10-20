// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include "scheduler.h"

//BEGIN changes by JJT
void CreateParentThread(int which);
//END changes by JJT
//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void
StartProcess(char *filename)
{
	// BEGIN changes made by JJT
	DEBUG('t', "test = %d\n", test);

	if(test == 1)
	{
		printf("Memory Allocation Method: First-Fit\n");
	}
	else if(test == 2)
	{
		printf("Memory Allocation Method: Best-Fit\n");
	}
	else if(test == 3)
	{
		printf("Memory Allocation Method: Worst-Fit\n");
	}
	else
	{
		printf("Memory Allocation Method: First-Fit\n");
	}
	// END changes made by JJT

	DEBUG('t',"inside start process \n");	
    OpenFile *executable = fileSystem->Open(filename);
	
    AddrSpace *space;
//BEGIN changes JJT
	initID();
//END changes JJT
	
	if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }
    space = new AddrSpace(executable); 

		
    currentThread->space = space;

    delete executable;			// close file
	DEBUG('t',"line54 right before fork in progtest.cc\n");
	//fork new thread
	
	space->InitRegisters();
	 //load page table register
	space->RestoreState();		
	//currentThread->Yield();
	machine->Run();	// jump to the user progam
	
	
//BEGIN changes by JJT
	
	
	ASSERT(FALSE);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
	DEBUG('t',"leaving start process \n");	
//END changes by JJT
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}
//BEGIN changes made by JJT

void
CreateParentThread(int which){
  	DEBUG('t',"enter CreateThread line265\n");
	DEBUG('t',"leave CreateThread line271\n");
}
//END changes by JJT

