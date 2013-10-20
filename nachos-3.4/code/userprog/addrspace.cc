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
#include "bitmap.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

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

int searchNum;
int searchReturn;
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
//	only uni programming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i, size;

	
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

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);

					
// BEGIN changes made by JJT

	
	


	// An array of Info data structure the size of the 
	// CoreMap->maxBits().
	storage = new Info[CoreMap->maxBits()];
	// Info data structure initialization, set everything to 0.
	//..data.addrStart = 0;
	//..data.size = 0;
	
	for(int r = 0; r < CoreMap->maxBits(); r++)
	{
		storage[r].addrStart = 0;
		storage[r].size = 0;
	}
	
	int j, temp;
	unsigned int count = 0;	// Variable to keep track of number of
			// contiguous memory space.
	bool available = FALSE;	// For now there is no space available.
	int k = 0;		// Starting array bit of storage array
	int m;
	int current = storage[0].size;
	
	StartPage = 0;		// Initialize StartPage to 0
				// will change whenever a fit method is
				// being run in the coming switch statement

	DEBUG('a', "StartPage = %d, line126 \n",StartPage);
	//CoreMap->Mark(5);
	//CoreMap->Mark(6);
	//CoreMap->Mark(15);
	//CoreMap->Mark(4);
	// searchNum = 0;
			//int TotalStorage[32] ={0} ;
		//	int sizeMax = 0;
	// SortMethod would be a global variable which would be given
	// whenever the user type the command line -M # or no -M, where
	// # would the sort method 1 = First-Fit, 2 = Best-Fit
	// 3 = Worst-Fit.
	switch(test)
	{
		//CoreMap->Mark(5);
		//CoreMap->Mark(6);
		//CoreMap->Mark(15);
		// First-Fit
		case 1:
								
			j = CoreMap->Find();	// Finds the first available space in BitMap
						// that is free.
								
			CoreMap->Clear(j);	// Find() sets the space to used but this 
						// will clear it back up because we don't know
						// yet if the process will start here.
						
			temp = j;		// Set the starting page (temp) to first available
						// spot open in CoreMap
						
			DEBUG('a', "Temp = %d, J = %d, StartPage = %d\n", 
					temp, j, StartPage); 
								


			
			// Iterate through CoreMap to see the first availabe
			// contiguous space in main memory by starting at first BitMap
			// location when CoreMap is set to 0.
 			for(int t = j; t < CoreMap->maxBits(); t++)
			{
			
				// If space t is not in use count would increase.
				// temp is here for remembering where the first available space 
				// was whenever count = 0 
				// if count == numPages, automatically set StartPage = temp
				// and stop manipulating StartPage to do page translation
				if(CoreMap->Test(t) == FALSE)
				{
					// If count equal 0 and a 0 is seen in CoreMap
					// this where one of the starting page location is at.
					if(count == 0)
						temp = t;
					
					// Increment the size of count to match the
					// free section in CoreMap					
					count++;
					
					// If count equal the size of a process, then the start
					// location of the process is temp.
					if(count == numPages)
					{
						StartPage = temp;
						break;
					}
				}
				
				// If space t is in use count would revert back to 0
				if(CoreMap->Test(t) == TRUE)
				{
					count = 0;
				}
			}
			
			// This if statement is for when the for loop is done and
			// StartPage is never assigned to temp because of not enough
			// contiguous memory space for this process.
			if(count < numPages)
			{
				DEBUG('t', "Out of Space!!\n");
				printf("Not Enough Space in Memory!!\nTerminating Process!!\n");
				delete storage;
				currentThread->Finish();	// Whatever function to terminate a process
								// whenever there is no room on main memory.
			}
			
			break;
			
		// Best-Fit
		case 2:	
			
			// Iterate through CoreMap and record where
			// there are available space in memory and their size (count)
			// above or equal to numPage.
			//printf("line 215 count = %d   ------------------------------\n",count);
			searchNum = 32;
			searchReturn = 0;
			for(j = 0; j < CoreMap->maxBits(); j++)
			{
				//printf("line 218 count = %d   ------------------------------\n",count);
				if(CoreMap->Test(j) == FALSE)
				{
					// If count is 0 and the CoreMap is 0 then
					// this is one of the start location of free space
					// in memory.
					if(count == 0)
						temp = j;
					// Increment the size of count to match the free
					// section in CoreMap.	
					count++;
					//printf(" line 227count = %d   ------------------------------\n",count);
					// Place an Info data structure in array storage
					// for later use whenever count is equal to or
					// greater than process size.
					if(count >= numPages)
					{
						//printf(" line 235 count = %d   ------------------------------\n",count);
						// Set available to true to say
						// that there is a free section in 
						// memory big enough for the process.
						available = TRUE;
						
						//data.addrStart = temp;
						//data.size = count;
						
						//storage[k] = data;
						storage[k].addrStart = temp;
						storage[k].size = count;
						
					//	printf(" number of pages = %d   \n",numPages);
					//	printf(" k = %d            \n",k);
					//	printf(" addrStart = %d, size = %d \n",storage[k].addrStart,storage[k].size);
						//printf(" addrStart = %d and size = %d ",storage[k].data.addrStart,storage[k].data.size);
						// Increment k so the next free section is 
						// placed in the next cell in array storage.
						k++;
					}
				}
				//printf("line 256 count = %d   ------------------------------\n",count);
				//searchNum = 0;
				
				if(CoreMap->Test(j) == TRUE)
				{
					//printf("line 259 count = %d   ------------------------------\n",count);
					// Place an Info data structure in array storage
					// for later use whenever count is equal to or
					// greater than process size and the current
					// CoreMap spot is set to 1.
					if(count >= numPages)
					{
						//printf(" line 266 count = %d   ------------------------------\n",count);
						// Set available to true to say
						// that there is a free section in
						// memory big enough for the process.
						available = TRUE;
						
						// data.addrStart = temp;
						// data.size = count;
						
						// storage[k] = data;
						storage[k].addrStart = temp;
						storage[k].size = count;
					
						k++;
					}
					// Revert count back to 0 because a 1 in CoreMap
					// was seen
					//count = 0;
					//printf("line 288 count = %d   ------------------------------\n",count);
				}
				
			}
			//printf("line 292 count = %d   ------------------------------\n",count);
			
			DEBUG('a', "count = %d, k = %d, j =%d, temp = %d \n",count, k,j,temp);
			printf( "count = %d, k = %d, j =%d, temp = %d \n",count, k,j,temp);
			// Terminates process gracefully because there is no 
			// available space on main memory
			if(count >= numPages)
			{
				StartPage = storage[0].addrStart;	
				// For loop to find the appropriate location where process will start.
				for(m = 1; m < CoreMap->maxBits(); m++)
				{
					// If the storage[m].size is less than current, set
					// StartPage to that starting address and set current to that
					// free section size.
					if(storage[m].size < current)
					{
						current = storage[m].size;
						StartPage = storage[m].addrStart;
						
					}
				}
				delete storage;
			}
			else
			{
				count = 0;
				DEBUG('t', "Out of Space!!\n");
				printf("Not Enough Space in Memory. \nTerminating Process.\n");
				delete storage;
				currentThread->Finish();// Whatever function to terminate a process
								// whenever there is no room on main memory.
				
			}
		
			 count = 0;
			 break; 
		// Worst-Fit
		// Very Similar to Best-Fit
		case 3:
			
			// Iterate through CoreMap and record where
			// there are available space in memory and their size (count)
			// above or equal to numPage.
			for(j = 0; j < CoreMap->maxBits(); j++)
			{
				if(CoreMap->Test(j) == FALSE)
				{
					// If count is 0 and the CoreMap is 0 then
					// this is one of the start location of free space
					// in memory.
					if(count == 0)
						temp = j;
					
					// Increment the size of count to match the free
					// section in CoreMap.
					count++;
					
					// Place an Info data structure in array storage
					// for later use whenever count is equal to or
					// greater than process size.
					if(count >= numPages)
					{
						// Set available to true to say
						// that there is a free section in
						// memory big enough for the process.
						available = TRUE;
						
						storage[k].addrStart = temp;
						storage[k].size = count;
					
						
						
						// Increment k so the next free section is
						// placed in the next cell in array storage
						k++;
					}
				}
				
				if(CoreMap->Test(j) == TRUE)
				{
					// Place an Info data structure in array storage
					// for later use whenever count is equal to or
					// greater than process size and the current
					// CoreMap spot is set to 1.
					if(count >= numPages)
					{
						available = TRUE;
						
						storage[k].addrStart = temp;
						storage[k].size = count;
					
						
						// Increment k so the next free section is
						// placed in the next cell in array storage
						k++;
					}
					// Revert count back to 0 because a 1 in CoreMap
					// was seen
					//count = 0;
				}
			}
			
			// Terminates process gracefully because there is no 
			// available space on main memory
			if(count < numPages)
			{
				DEBUG('t', "Out of Space.\n");
				printf("Not Enough Space in Memory.\nTerminating Process.\n");
				currentThread->Finish();	// Whatever function to terminate a process
								// whenever there is no room on main memory.
			}
		
			StartPage = storage[0].addrStart;
			// For loop to find the appropriate location where process will start.
			for(m = 1; m < CoreMap->maxBits(); m++)
			{
				// If the storage[m].size is less than current, set
				// StartPage to that starting address and set current to that
				// free section size.
				if(storage[m].size > current)
				{
					current = storage[m].size;
					StartPage = storage[m].addrStart;
					
				}
			} 
 			break;
			 
		// Same as First-Fit if no command line option (-M) was given. 
		default:		
								
			j = CoreMap->Find();	// Finds the first available space in BitMap
						// that is free.
								
			CoreMap->Clear(j);	// Find() sets the space to used but this 
						// will clear it back up because we don't know
						// yet if the process will start here.
						
			temp = j;		// Set the starting page (temp) to first available
						// spot open in CoreMap
								
			// Iterate through CoreMap to see the first availabe
			// contiguous space in main memory by starting at first BitMap
			// location when CoreMap is set to 0.
			for(int s = j; s < CoreMap->maxBits(); s++)
			{
				// If space s is not in use count would increase.
				// temp is here for remembering where the first available space 
				// was whenever count = 0 
				// if count == numPages, automatically set StartPage = temp
				// and stop manipulating StartPage to do page translation
				if(CoreMap->Test(s) == FALSE)
				{
					// If count equal 0 and a 0 is seen in CoreMap
					// this where one of the starting page location is at.
					if(count == 0)
						temp = s;
					
					// Increment the size of count to match the
					// free section in CoreMap	
					count++;
					
					// If count equal the size of a process, then the start
					// location of the process is temp.
					if(count == numPages)
					{
						StartPage = temp;
						break;
					}
				}
				
				// If space s is in use count would revert back to 0
				if(CoreMap->Test(s) == TRUE)
				{
					count = 0;
				}
			}
			
			// This if statement is for when the for loop is done and
			// StartPage is never assigned to temp because of not enough
			// contiguous memory space for this process.
			if(count < numPages)
			{
				DEBUG('t', "Out of Space.\n");
				printf("Not Enough Space in Memory.\nTerminating Process.\n");
				delete storage;
				currentThread->Finish();	// Whatever function to terminate a process
								// whenever there is no room on main memory.
			}
			
			break;
	}
	
// END changes made JJT

//int Target = 0;
	//Target = CoreMap->Find();
	 //DEBUG('t', " Target  = %d",Target);
				
// first, set up the translation //
	

	//delete storage;
	pageTable = new TranslationEntry[numPages];
	
    for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #

//BEGIN changes done by JJT

	pageTable[i].physicalPage = StartPage+i;
	
//END changes done by JJT

	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only

//BEGIN changes done by JJT

	CoreMap->Mark(StartPage+i);		// marking the bitmap
    }
    
    	//AccessMemory.V();
    
	CoreMap->Print(); 		// print out the bitmap
	
	// zero out the section in address space need for the new process.
	memset((machine->mainMemory+(PageSize*StartPage)),0,size);
	
//END changes done by JJT


// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
    
//BEGIN changes done by JJT

        DEBUG('a', "Initializing code segment, at 0x%x, size %d, StartPage %d, PageSize %d\n", 
			noffH.code.virtualAddr, noffH.code.size, StartPage, PageSize);
        
	executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr+(PageSize*StartPage)]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    
// the file is store in the (machine->mainMemory[noffH.code.virtualAddr]) location using the bcopy
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr+(PageSize*StartPage)]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
    
// END changes done by JJT
    
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Deallocate an address space.  
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
//BEGIN changes by JJT

  unsigned int i;

  DEBUG('t', "In Destructor--------------------------\n");

  for(i = 0; i < numPages; i++)
  {
  	CoreMap->Clear(pageTable[i].physicalPage);
  }

  CoreMap->Print();
 
  delete pageTable;
  
//BEGIN changes by JJT
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
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
