#ifndef THREAD_H_
#define THREAD_H_

#include "preprocessors.h"

extern const unsigned short STACK_SIZE;			 															// 120 bytes for saving registers and pushing values, 10 threads
extern const void* const baseAddress;												// base address for stacks

extern volatile unsigned short totalThreads;			 // At the start, there will be only one process and that is main process
extern volatile unsigned short currentThread;			/* Use for Round Robin Algorithm. Keeps track of which thread is running, 
																 	0 tells that only main process is running and no thread is created */

extern unsigned short threadPool[10]; // pool of ids (total 10 threads)
extern unsigned short threadID;
extern unsigned short threadIndex;

extern void* volatile pointer;							 // pointer = to hold function address

void Thread_Init(void);

// Interrupt service routines
void SysTick_Handler(void);
void PendSV_Handler(void);

int Create_Thread(void* funcAddr);				//Creates a thread, return 0 for unsuccessfull, else it returns thread ID
int Delete_Thread(int id);		//Deletes a thread, returns 0 for unsuccessful and 1 for success

#endif