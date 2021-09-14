#include "synchronization.h"


const int SEM_LIMIT = 11;


void mutex_init(Mutex* obj)
{
	(*obj).mut_var = 1;
}

void mutex_lock(Mutex* obj)
{	
	__asm(		
	// decrementing mut_var
	"PUSH {R1}\n\t"
	"ACQUIRE_MUTEX_LOCK:\n\t"
		"LDREX R1, [R0,#0]\n\t"
		"CMP R1, #0\n\t"
		"BEQ ACQUIRE_MUTEX_LOCK\n\t"									// Wait for lock to release
	
		"SUB R1,#1\n\t"
		"STREX R1, R1, [R0,#0]\n\t"
		"CMP R1, #0\n\t"
		"BNE ACQUIRE_MUTEX_LOCK\n\t"
	"POP {R1}\n\t"
	);
}

void mutex_unlock(Mutex* obj)
{
	__asm(		
	"PUSH {R1}\n\t"
	"LDR R1, [R0,#0]\n\t"
	"ADD R1,#1\n\t"	
	"STR R1, [R0,#0]\n\t"
	"POP {R1}\n\t"	
	);
}


int sem_init(Semaphore* sem_obj, int value)				// returns 1 on success, else returns 0
{
	if(value <= SEM_LIMIT)
	{
		(*sem_obj).sem_var = value;
		return 1;
	}
	return 0;
}

void sem_wait(Semaphore* sem_obj)
{	
	__asm(
		"PUSH {R1}\n\t"
	  "ACQUIRE_SEM_LOCK:"
			"LDREX R1, [R0,#0]\n\t"
			"CMP R1, #0\n\t"
			"BLE ACQUIRE_SEM_LOCK\n\t"
			"SUB R1, #1\n\t"
			"STREX R1, R1,[R0,#0]\n\t"
			"CMP R1, #0\n\t"
			"BNE ACQUIRE_SEM_LOCK\n\t"
		"POP {R1}\n\t"
	);
}

void sem_signal(Semaphore* sem_obj)
{
	__asm(
		"PUSH {R1}\n\t"
	  "RELEASE_SEM_LOCK:"
			"LDREX R1, [R0, #0]\n\t"
			"ADD R1, #1\n\t"
			"STREX R1, R1, [R0,#0]\n\t"
			"CMP R1, #0\n\t"
			"BNE RELEASE_SEM_LOCK\n\t"
		"POP {R1}\n\t"
	);
}

int sem_getValue(Semaphore* sem_obj)
{
	return ((*sem_obj).sem_var);
}