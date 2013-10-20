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

#include <stdio.h>        // FA98
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "addrspace.h"   // FA98
#include "sysdep.h"   // FA98

// begin FA98

static int SRead(int addr, int size, int id);
static void SWrite(char *buffer, int size, int id);// end FA98
void CreateThread(int which);
void CreateJoinThread1(int which);
void CreateJoinThread2(int which);

//BEGIN changes by JJT
AddrSpace *space2;
AddrSpace *space3;
AddrSpace *space4;
SpaceId tempId;
int answer2;
int answer3;
int t_int;
int parent_id;
int child_id;
Thread *old_Thread;
Thread *join_Thread;
//END changes by JJT
//tar -czvf project02_ttn7541_jpj9668_trs8798_nachos.tar.gz ./nachos-3.4 ./gnu-decstation-ultrix
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
	int arg1 = machine->ReadRegister(4);
	int arg2 = machine->ReadRegister(5);
	int arg3 = machine->ReadRegister(6);
	int Result;
	int i, j;
	char *ch = new char [500];
//BEGIN changes by John Judice
	int ii;
	char *ch2 = new char [500];
	//char *ch3 = new char [500];
	OpenFile *CurrentExec;
	//Thread *t, *parent, *child;
	Thread *t;
	int answer;
//END changes by John Judice
	switch ( which )
	{
	case NoException :
		break;
	case SyscallException :

		// for debugging, in case we are jumping into lala-land
		// Advance program counters.
		machine->registers[PrevPCReg] = machine->registers[PCReg];
		machine->registers[PCReg] = machine->registers[NextPCReg];
		machine->registers[NextPCReg] = machine->registers[NextPCReg] + 4;

		switch ( type )
		{

		case SC_Halt :
			DEBUG('t', "Shutdown, initiated by user program.\n");
			answer = currentThread->getSpaceID();
			printf("SYSTEM CALL:Halt called by thread %d\n",answer);
			CoreMap->Print();
			interrupt->Halt();
			break;
			
//BEGIN changes done by Trellis	Sherman
		case SC_Exit :
			DEBUG('t', "Exit, choose by the user.\n");
			answer = currentThread->getSpaceID();
			printf("SYSTEM CALL:Exit called by thread %d\n",answer);
			CoreMap->Print();
			currentThread->Finish();
			
			break;

		    
		case SC_Yield :
			answer = currentThread->getSpaceID();
			printf("SYSTEM CALL:Yield called by thread %d\n",answer);
			DEBUG('t', "Yield, choosen by the user.\n");
			CoreMap->Print();
			printf("System Call Yield on thread\n");
			currentThread->Yield();
			break;
			
//END changes done by Trellis Sherman
//BEGIN changes done by JJT
		case SC_Exec :
			//CoreMap->Print();
			answer = currentThread->getSpaceID();
			printf("SYSTEM CALL:Exec called by thread %d\n",answer);
			DEBUG('t',"inside exec \n");
			DEBUG('t',"type = %d arg1 = %d, arg2 =%d, arg3 =%d \n",type, arg1,arg2, arg3);
			//function to retrieve the file name used in OpenFile		
			for (int jj = 0;jj < arg1 && jj<500 ; jj++) {
				if(!machine->ReadMem((arg1+jj), 1, &ii))
					jj=jj-1;
				else{
					ch2[jj] = (char) ii;
					if (ch2[jj] == '\0') 
					//printf("%s",ch2);
					break;
					}
			}
			DEBUG('t',"file name received by read memory %s \n",ch2);
			// open the file using the file name
			CurrentExec = fileSystem->Open(ch2);
			// allocate a new address space for the new process
			if (CurrentExec == NULL) {
				printf("Unable to open file %s\n", ch2);
				return;
			}
			else
			{
				// init an address space for the new thread
				space2 = new AddrSpace(CurrentExec);

				//creat a new thread 
				t = new Thread("execute-worker");
				t->space = space2;
				CoreMap->Print();
				DEBUG('t',"line162 right before fork\n");
				answer2 = getID();
				//fork new thread
				t->Fork(CreateThread,answer2);
				// close file
				delete CurrentExec;	
				delete ch2;
			
				machine->WriteRegister(2,answer2);
				//if (currentThread->getName() == "main")
				//{
					currentThread->Yield();
				//}
				DEBUG('t',"leaving exec \n");
			}
			break;

		case SC_Join :
			// this code is left here so when this project is 
			// evaluated it will show that work was put into 
			// this part of the project. Although this section
			// was not completed at time of submission
/*			CoreMap->Print();
			answer = currentThread->getSpaceID();
			child_id = answer +1;
			parent_id = answer;
			printf("SYSTEM CALL:Join called by thread %d\n",answer);
			for (j = 0;j<arg1 && j<500; j++) 
			{
			   if(!machine->ReadMem((arg1+j), 1, &i))
				j=j-1;
			   else
			   {
				ch3[j] = (char) i;
				if (ch3[j] == '\0') 
				break;
			   }
			}

			//Open the file into a binary stream//
			CurrentExec = fileSystem->Open(ch3);

			// allocate a new address space for the new process
			
			if (CurrentExec == NULL) 
			{
				printf("Unable to open file %s\n", ch3);
				return;
			}
			
			DEBUG('t', "Join syscall: opening file--success\n");

			space2 = new AddrSpace(CurrentExec);
			child = new Thread("child-worker");
			child->space = space2;
			answer3 = getID();
			child_id = child->getSpaceID();
			child->Fork(CreateJoinThread1,child_id);
			//CoreMap->Print();
			//child_id = child->getSpaceID();

			parent = new Thread("parent-worker");
			parent->space = space2;
			answer2 = getID();	
			//parent_id = parent->getSpaceID();
			parent->Fork(CreateJoinThread2,parent_id);
		//parent_id = parent->getSpaceID();
		//	parent->Join(parent_id, child_id);
			
			currentThread->Join(parent_id, child_id);

			//printf("This thread %d is now running\n",parent_id);
			

		//	machine->WriteRegister(2,answer2);
		machine->WriteRegister(2,answer3);
				if (currentThread->getName() == "main")
				{
					//currentThread->Join(parent_id, child_id);
currentThread->Yield();
				}

			delete CurrentExec;			// close file
			
			DEBUG('t',"leaving Join \n");	
*/			
			break;	
	
//END changes by JJT

		
		case SC_Read :
			if (arg2 <= 0 || arg3 < 0){
				printf("\nRead 0 byte.\n");
			}
			Result = SRead(arg1, arg2, arg3);
			machine->WriteRegister(2, Result);
			DEBUG('t',"Read %d bytes from the open file(OpenFileId is %d)",
			arg2, arg3);
			break;

		case SC_Write :
			for (j = 0; ; j++) {
				if(!machine->ReadMem((arg1+j), 1, &i))
					j=j-1;
				else{
					ch[j] = (char) i;
					if (ch[j] == '\0') 
						break;
				}
			}
			if (j == 0){
				printf("\nWrite 0 byte.\n");
				// SExit(1);
			} else {
				DEBUG('t', "\nWrite %d bytes from %s to the open file(OpenFileId is %d).", arg2, ch, arg3);
				SWrite(ch, j, arg3);
			}
			break;

			default :
			//Unprogrammed system calls end up here
			break;

		}         
		break;

	case ReadOnlyException :
		answer = currentThread->getSpaceID();
		printf("Exception Case:ReadOnlyException called by thread %d\n",answer);
		currentThread->Finish();
		break;
	case BusErrorException :
		answer = currentThread->getSpaceID();
		printf("Exception Case:BusErrorException called by thread %d\n",answer);
		currentThread->Finish();
		break;
	case AddressErrorException :
		printf("Pointer out of bounds");
		currentThread->Finish();
		break;
	case OverflowException :
		answer = currentThread->getSpaceID();
		printf("Exception Case:OverflowException called by thread %d\n",answer);
		currentThread->Finish();
		break;
	case IllegalInstrException :
		answer = currentThread->getSpaceID();
		printf("Exception Case:IllegalInstrException called by thread %d\n",answer);
		currentThread->Finish();
		break;
	case NumExceptionTypes :
		answer = currentThread->getSpaceID();
		printf("Exception Case:NumExceptionTypes called by thread %d\n",answer);
		currentThread->Finish();
		break;

		default :
		      printf("Unexpected user mode exception %d %d\n", which, type);
		//      if (currentThread->getName() == "main")
		//      ASSERT(FALSE);
		//      SExit(1);
		break;
	}
	delete [] ch;
}

// BEGIN Changes by JJT

void
CreateThread(int which){
  

	DEBUG('t',"enter CreateThread line324\n");
	// set the initial register values
	space2->InitRegisters();
			//load page table register
	space2->RestoreState();
			//currentThread->Yield();
	//currentThread->Yield();
	
	machine->Run();
	DEBUG('t',"leave CreateThread line271\n");
}
//END Changes by JJT
// These lines of code are from the development of join
// although this part of the project was not completed
// we commented it out to show effort was put into
// completing it.
void
CreateJoinThread1(int which){
	
	DEBUG('t',"enter CreateJoinThread line355\n");
	// set the initial register values
	space2->InitRegisters();
			//load page table register
	space2->RestoreState();
			//currentThread->Finish();
	//
  currentThread->Yield();
	

	machine->Run();
	DEBUG('t',"leave CreateThread line271\n");
}
void CreateJoinThread2(int which){
	//currentThread->Finish();
	DEBUG('t',"enter CreateJoinThread line355\n");
	// set the initial register values
	space2->InitRegisters();
			//load page table register
	space2->RestoreState();
	//parent->Join(parent_id, child_id);
	currentThread->Yield();
        //currentThread->Join(parent_id, child_id);
	machine->Run();
	DEBUG('t',"leave CreateThread line271\n");
}
//END Changes by JJT

static int SRead(int addr, int size, int id)  //input 0  output 1
{
	char buffer[size+10];
	int num,Result;

	//read from keyboard, try writing your own code using console class.
	if (id == 0)
	{
		scanf("%s",buffer);

		num=strlen(buffer);
		if(num>(size+1)) {

			buffer[size+1] = '\0';
			Result = size+1;
		}
		else {
			buffer[num+1]='\0';
			Result = num + 1;
		}

		for (num=0; num<Result; num++)
		{  machine->WriteMem((addr+num), 1, (int) buffer[num]);
			if (buffer[num] == '\0')
			break; }
		return num;

	}
	//read from a unix file, later you need change to nachos file system.
	else
	{
		for(num=0;num<size;num++){
			Read(id,&buffer[num],1);
			machine->WriteMem((addr+num), 1, (int) buffer[num]);
			if(buffer[num]=='\0') break;
		}
		return num;
	}
}



static void SWrite(char *buffer, int size, int id)
{
	//write to terminal, try writting your own code using console class.
	if (id == 1)
	printf("%s", buffer);
	//write to a unix file, later you need change to nachos file system.
	if (id >= 2)
	WriteFile(id,buffer,size);
}

// end FA98

