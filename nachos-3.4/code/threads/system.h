// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"

// BEGIN changes made by JJT

#include "synch.h"

// END changes made by JJT

// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics/
extern Timer *timer;				// the hardware alarm clock

//BEGIN changes by JJT
extern int getID();
extern void	incID(); 
extern void initID();

extern int test;

//extern Semaphore AccessMemory;

//End changes by JJT

#ifdef USER_PROGRAM
#include "machine.h"

extern Machine* machine;   // user program memory and registers
//BEGIN changes by JJT
#include "bitmap.h"


extern BitMap * CoreMap;	// bitmap used in addrspace

//END changes by JJT
#endif

#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"//
extern FileSystem  *fileSystem;

#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk   *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"/
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
