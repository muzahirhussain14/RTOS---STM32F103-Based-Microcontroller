/*
	Context Switching is almost complete. Just need to use locks before accessing global variables in each thread...
*/


#define STK_CTRL     (*((volatile unsigned long*) 0xE000E010))	// sysTick configuration register
#define STK_LOAD     (*((volatile unsigned long*) 0xE000E014))					// sysTick load value register
#define STK_VAL      (*((volatile unsigned long*) 0xE000E018))					// systick Current Value Register(Not Used)
	
#define STK_CALIB    (*((volatile unsigned long*) 0xE000E01C))					/*  SysTick Calibration Value Register is a read-only register
																																						that contains the number of pulses for a period of 10ms.
																																						We can use this Register to scale your value for the 
																																						Reload register, to generate an event at a timed interval,
																																						for example 1ms (Not Used)*/

#define ICSR   		    (*((volatile unsigned long*) 0xE000ED04))					/*  Interrupt Controll and State register
																																						Used to set/ disable PendSV exception 
																																						Bit 27: pendSv clear 
																																						Bit 28: pendSv set	*/

#define GPIOD_CRL    (*((volatile unsigned long*) 0x40011400))
#define GPIOD_BSRR   (*((volatile unsigned long*) 0x40011410))	
	
#define GPIOC_CRL    (*((volatile unsigned long*) 0x40011000))
#define GPIOC_BSRR   (*((volatile unsigned long*) 0x40011010))	

#define GPIOB_CRL    (*((volatile unsigned long*) 0x40010C00))
#define GPIOB_BSRR   (*((volatile unsigned long*) 0x40010C10))		

#define GPIOA_CRL    (*((volatile unsigned long*) 0x40010800))
#define GPIOA_BSRR   (*((volatile unsigned long*) 0x40010810))	


const unsigned int STACK_SIZE = 120;			 		// 120 bytes for saving registers and pushing values, 10 threads

const void* const baseAddress = (void*)0x20004A38;		// base address for stacks

volatile unsigned int totalProcesses = 1;			// At the start, there will be only one process and that is main process
volatile unsigned int currentProcess = 0;			/* keeps track of which process is running, 
																							 	0 tells that only main process is running and no task is created */
unsigned int threadID = 0;			 							// ID of the thread

void* volatile pointer = 0;							 // pointer = to hold function address
volatile int counter = 0;								// rough work, delete it afterwards


/*
__attribute__((noreturn)):
	This command when used with function prototype, informs the compiler that the function 
	does not return. The compiler can then perform optimizations by removing code that is never reached.

	void overflow(void) __attribute__((noreturn)); // called on overflow
 
	void overflow(void)
	{
    __asm {
        SVC 0x123; // hypothetical exception-throwing system service
    }
  
		while (1);
	}
	Create Thread Function using this.
*/

void Init()
{
	// LED INIT
	GPIOD_CRL &= 0xFFFFFF0F;
	GPIOD_CRL |= 0x00000020;
	
	GPIOC_CRL &= 0xFFFFFF0F;
	GPIOC_CRL |= 0x00000020;
	
	GPIOB_CRL &= 0xFFFFFF0F;
	GPIOB_CRL |= 0x00000020;
	
	GPIOA_CRL &= 0xFFFFFF0F;
	GPIOA_CRL |= 0x00000020;
	
	
	STK_LOAD = 0x000000FF;					// Value in Load register
	
	//start the timer...
	STK_CTRL = 0x00000007;				// Use Processor Clock, asserts the SysTick exception request, SysTick Start
}


void SysTick_Handler(void)			// Interrupt service routines
{
	++counter;
	
	if(totalProcesses > 1)				// If there is more then 1 process, then switch context
	{
		
		ICSR |= 0x10000000;					// Pend SV interrupt for context switching
	}
}


/* The reason for performing the context switch inside the PendSV is because the
	 pendSv has the lowest priority and will run after all other exceptions(if any)*/
void PendSV_Handler(void)
{	
			//Stopping SysTick timer
			STK_CTRL = 0x00000000;
	
			//Saving StackPointer
			__asm(
				"PUSH {R4, R5, R6, R7, R8, R9, R10, R11}\n\t"			// Pushing all remaining registers inside the stack
	
				".global baseAddress\n\t"					// Accessing C global variable, "baseAddress"
				".global STACK_SIZE\n\t"
			  ".global currentProcess\n\t"
				
				"LDR R0, =baseAddress\n\t"				// base address of stacks
				"LDR R0, [R0,#0]\n\t"
				
				"LDR R1, =STACK_SIZE\n\t"
				"LDR R1, [R1, #0]\n\t"						// size of single stack
				
				"LDR R2, =currentProcess\n\t"
				"LDR R2, [R2, #0]\n\t"						// currentProcess							
				
				//SAVING STACK POINTER
				"SUB R0, R1\n\t"									// subtraction 60 bytes from base address to make calculations easy
				"MUL R2, R2, R1\n\t"							// offset to the next stack
				"ADD R0, R2\n\t"									// adding offset to base address to get to the current stack
				"ADD R0, R1\n\t"									// getting to the stackpointer value of the new stack
				"STR R13, [R0, #0]\n\t"						// Saving the stackpointer value of current process
			);
		
		  currentProcess = (currentProcess + 1) % totalProcesses;			// move on to the next process
			
			//Load StackPointer		
			__asm(
				".global baseAddress\n\t"					// Accessing C global variable, "baseAddress"
				".global STACK_SIZE\n\t"
			  ".global currentProcess\n\t"
				
				"LDR R0, =baseAddress\n\t"				// base address of stacks
				"LDR R0, [R0,#0]\n\t"
				
				"LDR R1, =STACK_SIZE\n\t"
				"LDR R1, [R1, #0]\n\t"						// size of single stack
				
				"LDR R2, =currentProcess\n\t"
				"LDR R2, [R2, #0]\n\t"						// currentProcess							
				
				//Loading STACK POINTER
				"SUB R0, R1\n\t"									// subtraction 100(Size of stack) bytes from base address to make calculations easy
				"MUL R2, R2, R1\n\t"							// offset to the next stack
				"ADD R0, R2\n\t"									// adding offset to base address to get to the next stack
				"ADD R0, R1\n\t"									// getting to the stackpointer value of the this next stack
				"LDR R13, [R0, #0]\n\t"						// Reading the stackpointer value of current process
			
				"POP {R4, R5, R6, R7, R8, R9, R10, R11}\n\t"			// Loading all remaining registers
				
				//starting the timer...
				"LDR R0, =0xE000E010\n\t"
				"MOV R1, 0x00000007\n\t"					// Use Processor Clock, asserts the SysTick exception request, SysTick Start
				"STR R1, [R0, #0]\n\t"
			);			
}

int Create_Task(void* funcAddr)					// return 0 for unsuccessfull, else it return thread ID
{	
		if(totalProcesses > 10)							// Memory out of range, No more then 10 Threads [1-10]
			return 0;
	
		else
		{
			STK_CTRL &= 0xFFFFFFFD;															// disable the timer interrupt, timer will still continue
			
			
			if(threadID == 4294967295u)													// this is the maximum value an unsigned integer can have, 'u' represents "unsigned"
				threadID = 0;
			
			threadID = threadID + 1;														// generating id for next process
			pointer = funcAddr;
			
			__asm volatile(
				"PUSH {R0, R1, R2, R3}\n\t" 											/* Usually inline assembly disrupts the flow of the program, 
																														 therefore pushing important register to save the before state */ 
						
				".global totalProcesses\n\t"											// Accessing C global variable, "totalProcesses"
				".global pointer\n\t"															// Accessing C global variable, "pointer", holding func. address
				".global baseAddress\n\t"													// Accessing C global variable, "baseAddress"
				".global STACK_SIZE\n\t"													// Accessing C global variable, "stack_size"
				".global threadID\n\t"														// Accessing C global variable, "threadId"
				
			
				"LDR R1, =totalProcesses\n\t"											// load totalProcesses address to r1		
				"LDR R1, [R1, #0]\n\t"														// mov the value of totalProcesses to r1
				"SUB R1, 1\n\t"																		// subtract 1 from total process for calculation
			
				"LDR R2, =pointer\n\t"														// load pointer address in r2, pointer holds the address of the function
				"LDR R2, [R2, #0]\n\t"														// load pointer contents (i.e. function address) in r2
				
				"LDR R3, =STACK_SIZE\n\t"
				"LDR R3, [R3, #0]\n\t"														// size of single stack
			
				"LDR R0, =baseAddress\n\t"												// base address of stacks
				"LDR R0, [R0,#0]\n\t"
				"ADD R0, 4\n\t"																		/* adding 4 bytes to base address, because first four bytes are reserved for
																														 main process stack pointer value */	
				
				"MUL R1, R1, R3\n\t"															// calculating the offset to get to the new stack		
				"ADD R0, R1\n\t"																	// adding the offset to base address of stacks to get to new stack
								
				"MOV R1, 0x00000000\n\t"				
				"STR R1, [R0, #48]\n\t"														// R4 = 0
				"STR R1, [R0, #52]\n\t"														// R5 = 0
				"STR R1, [R0, #56]\n\t"														// R6 = 0
				"STR R1, [R0, #60]\n\t"														// R7 = 0
				"STR R1, [R0, #64]\n\t"														// R8 = 0
				"STR R1, [R0, #68]\n\t"														// R9 = 0
				"STR R1, [R0, #72]\n\t"														// R10 = 0
				"STR R1, [R0, #76]\n\t"														// R11 = 0
				
				"STR R1, [R0, #80]\n\t"														// R0 = 0
				"STR R1, [R0, #84]\n\t"														// R1 = 0
				"STR R1, [R0, #88]\n\t"														// R2 = 0
				"STR R1, [R0, #92]\n\t"														// R3 = 0								
				"STR R1, [R0, #96]\n\t"														// R12 = 0
				
				"STR R14, [R0, #100]\n\t"													// Link Register	
				"STR R2, [R0, #104]\n\t"													// PC
				
				"MOV R1, 0x01000000\n\t"			
				"STR R1, [R0, #108]\n\t"													// PSR
																													/* Program Status Register:
																														No flag bit set in new process, 
																														new process is in thumb state,
																														new process is in thread mode */
				"LDR R1, =threadID\n\t"
				"LDR R1, [R1, #0]\n\t"
				"STR R1, [R0, #112]\n\t"													// Storing Thread_Id
				
				"MOV R1, R0\n\t"																	// Mov baseAddress + 4 to R1
				"ADD R1, #48\n\t"																	// pointing to the top of the stack
				"STR R1, [R0, #116]\n\t"													// storing the value of Stackpointer

				// incrementing totalProcesses
				"LDR R1, =totalProcesses\n\t"											// load totalProcesses address to r1		
				"LDR R0, [R1, #0]\n\t"														// mov the value of totalProcesses to r0
				"ADD R0,1\n\t"
				"STR R0,[R1, #0]\n\t"
				
				"POP {R0, R1, R2, R3}\n\t"
				);
				
			STK_CTRL |= 0x00000002;															// renable the timer interrupt
			
			pointer = 0;																				 // Nullify the pointer
			return threadID;
		}
}

int Delete_Task(int id)																			// return 0 for unsuccessful and 1 for successful
{			
		if(id <= 0)
			return 0;		
				
		int temp = totalProcesses;
		
		if(totalProcesses > 1)
		{
			STK_CTRL &= 0xFFFFFFFD;															// disable the timer interrupt, timer will still continue
			pointer = &id;
			
			// search for id and delete that process
			__asm volatile(
	
				"PUSH {R0, R1, R2, R3, R4, R5, R6, R7}\n\t" 			/* Usually inline assembly disrupts the flow of the program, 
																														therefore pushing important register to save the before state */
				".global pointer\n\t"															// Accessing C global variable, "pointer", holding func. address			
				".global totalProcesses\n\t"											// Accessing C global variable, "totalProcesses"
				".global baseAddress\n\t"													// Accessing C global variable, "baseAddress"
				".global STACK_SIZE\n\t"													// Accessing C global variable, "stack_size"
				".global threadID\n\t"														// Accessing C global variable, "threadId"				
				
				"LDR R0, =pointer\n\t"														// load pointer address in r0, pointer holds the address of the variable
				"LDR R0, [R0, #0]\n\t"														// load pointer contents (i.e.variable address) in r0
				"LDR R0, [R0, #0]\n\t"														// load variable contents (i.e.variable value) in r0
			
				"LDR R1, =totalProcesses\n\t"											// load totalProcesses address to r1		
				"LDR R1, [R1, #0]\n\t"														// mov the value of totalProcesses to r1
				"SUB R1, R1, #1\n\t"
			
				"LDR R2, =STACK_SIZE\n\t"
				"LDR R2, [R2, #0]\n\t"														// size of single stack
				
				"LDR R3, =baseAddress\n\t"												// base address of stacks
				"LDR R3, [R3,#0]\n\t"
				"ADD R3, 4\n\t"																		/* adding 4 bytes to base address, because first four bytes are reserved for
																															main process stack pointer value */				
			
				"LOOP_1:\n\t"
				
					"LDR R4, [R3, #112]\n\t"												// Moving the ID value to R4\n\t"
					"CMP R4, R0\n\t"																// comparing the id
					"BEQ FOUND\n\t"
					
					"ADD R3, R2\n\t"																// Move on to next stack
					"SUBS R1,R1,#1\n\t"														  /* SUB doesn't update flags. SUBS does.
																														 SUBS update the N, Z, C and V flags*/
				"BNE  LOOP_1\n\t"																	// if R1 is not equal to 0 it will jumb to loop_1
				"B Exit_2\n\t"																		// ID not Found				
				
				"FOUND:\n\t"																			// ID found
									
					"CMP R1, #1\n\t"
					"BEQ Exit_1\n\t"																// if we have to delete last stack, simple decrement totalprocesses.
					
					// copy last stack to current stack and decrement current process					
					"LDR R1, =totalProcesses\n\t"											// load totalProcesses address to r1		
					"LDR R1, [R1, #0]\n\t"														// mov the value of totalProcesses to r1
					"SUB R1, R1, #2\n\t"
					
					"LDR R0, =baseAddress\n\t"												// base address of stacks
					"LDR R0, [R0,#0]\n\t"
					"ADD R0, 4\n\t"
						
					"MUL R2, R2, R1\n\t"
					"ADD R0, R2\n\t"																	// pointing to the last stack	
					
					"MOV R5, #0\n\t"
					"MOV R1, #30\n\t"																	// loop 30 to 1 -> copy all 30 elements from last stack to current stack
					
					"LOOP_2:\n\t"
						"LDR R6, [R0, R5]\n\t"													// loading value from last stack to r6
						"STR R6, [R3, R5]\n\t"													// storing valuse from variable to current stack
						"ADD R5, #4\n\t"																// Moving to next element
						"SUBS R1,R1,#1\n\t"														  
					"BNE  LOOP_2\n\t"
					
					// calculating stack pointer value
					"MOV R7, R0\n\t"																	// load last stack address to r7
					"SUB R7, R7, R3\n\t"															// Subtract last stack base address with current stack base address
					"SUB R6, R6, R7\n\t"															// calculating new stack pointer value for last stack	for this new location
					"STR R6, [R3, #116]\n\t"													// storing the stack pointer value according to new memory location
					
				"Exit_1:\n\t"	
					// decrement totalProcesses
					"LDR R1, =totalProcesses\n\t"											
					"LDR R0, [R1, #0]\n\t"														
					"SUB R0, R0, #1\n\t"
					"STR R0, [R1, #0]\n\t"
					
				"Exit_2:\n\t"		
					"POP {R0, R1, R2, R3, R4, R5, R6, R7}\n\t"					
			);
			pointer = 0;			
			STK_CTRL |= 0x00000002;															// renable the timer interrupt
		}
		
		if(totalProcesses < temp)					// if totalProcesses is decremented, it means the process was deleted successfully
		{
			if(totalProcesses == 1)					// if there are no more processes
					threadID = 0;
			return 1;
		}
		
		else
			return 0;
}

int task_1, task_2, task_3, task_4, task_5, task_6, task_7, task_8, task_9, task_10;

void LED_On(void) __attribute__((noreturn));
void LED_ON_1(void) __attribute__((noreturn));
void LED_ON_2(void) __attribute__((noreturn));
void LED_ON_3(void) __attribute__((noreturn));
void LED_ON_4(void) __attribute__((noreturn));

void LED_Off(void) __attribute__((noreturn));
void LED_OFF_1(void) __attribute__((noreturn));
void LED_OFF_2(void) __attribute__((noreturn));
void LED_OFF_3(void) __attribute__((noreturn));
void LED_OFF_4(void) __attribute__((noreturn));


void LED_Off(void)
{	
	while(1)
		GPIOB_BSRR = 0x00020000;
}

void LED_On(void)
{		
	int flag_1 = 1;
	
	__asm(
		"PUSH {R0, R2, R3, R4}\n\t"
	);
	
	while(1)
	{
		GPIOB_BSRR = 0x00000002;
			
		if(counter > 300000 && flag_1 == 1)
		{
			task_2 = Create_Task(LED_Off);						
			flag_1 = 0;
		}
	}	
}

void LED_OFF_1(void)
{
	while(1)
		GPIOA_BSRR = 0x00020000;
}

void LED_ON_1(void)
{
	int flag_1 = 1;
	
	__asm(
		"PUSH {R0, R2, R3, R4}\n\t"
	);
	
	
	while(1)
	{
		GPIOA_BSRR = 0x00000002;
	
		if(counter > 300000 && flag_1 == 1)
		{
			task_4 = Create_Task(LED_OFF_1);			
			flag_1 = 0;
		}
	}	
}

void LED_OFF_2(void)
{
	while(1)
		GPIOC_BSRR = 0x00020000;
}

void LED_ON_2(void)
{
	int flag_1 = 1;

		__asm(
		"PUSH {R0, R2, R3, R4}\n\t"
	);
	
	while(1)
	{
		GPIOC_BSRR = 0x00000002;
	
		if(counter > 300000 && flag_1 == 1)
		{
			task_6 = Create_Task(LED_OFF_2);			
			flag_1 = 0;
		}
	}
}

void LED_OFF_3(void)
{	
	while(1)
		GPIOD_BSRR = 0x00020000;
}

void LED_ON_3(void)
{	
	int flag_1 = 1;
	
			__asm(
		"PUSH {R0, R2, R3, R4}\n\t"
	);	
	
	while(1)
	{
		GPIOD_BSRR = 0x00000002;
		
		if(counter > 300000 && flag_1 == 1)
		{
			task_8 = Create_Task(LED_OFF_3);			
			flag_1 = 0;
		}
	}
}

void LED_OFF_4(void)
{
	while(1)
		GPIOA_BSRR = 0x00010000;
}

void LED_ON_4(void)
{
	int flag_1 = 1;
	
			__asm(
		"PUSH {R0, R2, R3, R4}\n\t"
	);
	
	while(1)
	{
		GPIOA_BSRR = 0x00000001;
		
		if(counter > 300000 && flag_1 == 1)
		{
			task_10 = Create_Task(LED_OFF_4);			
			flag_1 = 0;
		}
	}
}


int main(void)
{		
	Init();		
		
	task_1 = Create_Task(LED_On);
	task_2 = Create_Task(LED_Off);
	
	Delete_Task(task_1);
	task_1 = Create_Task(LED_On);
	
	Delete_Task(task_2);
	task_2 = Create_Task(LED_Off);
	
	Delete_Task(task_1);
	task_1 = Create_Task(LED_On);
	
	Delete_Task(task_2);
	task_2 = Create_Task(LED_Off);
		
	task_3 = Create_Task(LED_ON_1);
	task_4 = Create_Task(LED_OFF_1);
	
	task_5 = Create_Task(LED_ON_2);
	task_6 = Create_Task(LED_OFF_2);
	
	task_7 = Create_Task(LED_ON_3);
	task_8 = Create_Task(LED_OFF_3);
	
	task_9 = Create_Task(LED_ON_4);
	task_10 = Create_Task(LED_OFF_4);
		
	int flag = 1;
	int flag_1 = 1;
	int flag_2 = 1;
	int flag_3 = 1;
	
	while(1)
	{
	
		if(counter > 200000 && flag == 1)
		{
			__asm(
			"PUSH {R1-R12}\n\t"
			"MOV R1, #0\n\t"
			"MOV R2, #0\n\t"
			"MOV R3, #0\n\t"
			"MOV R4, #0\n\t"
			"MOV R5, #0\n\t"
			"MOV R6, #0\n\t"
			"MOV R7, #0\n\t"
			"MOV R8, #0\n\t"
			"MOV R9, #0\n\t"
			"MOV R10, #0\n\t"
			"MOV R11, #0\n\t"
			"MOV R12, #0\n\t"
			"POP {R1-R12}\n\t"
			);
			
			Delete_Task(task_2);
			Delete_Task(task_4);
			Delete_Task(task_6);
			Delete_Task(task_8);
			Delete_Task(task_10);
			
			flag = 0;
		}
		
			
		if(counter > 800000 && flag_3 == 1)
		{			
			Delete_Task(task_1);
			Delete_Task(task_2);
			Delete_Task(task_3);
			Delete_Task(task_4);
			Delete_Task(task_5);
			Delete_Task(task_6);
			Delete_Task(task_7);
			Delete_Task(task_8);
			Delete_Task(task_9);
			Delete_Task(task_10);
			
			flag_3 = 0;
		}
	}
	return (1);
}
	
void SystemInit(void)
{
}