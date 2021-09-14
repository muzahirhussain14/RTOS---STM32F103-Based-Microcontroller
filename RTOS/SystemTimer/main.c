#define STK_CTRL     (*((volatile unsigned long*) 0xE000E010))					// sysTick configuration register
#define STK_LOAD     (*((volatile unsigned long*) 0xE000E014))					// sysTick load value register
#define STK_VAL      (*((volatile unsigned long*) 0xE000E018))					// systick Current Value Register(Not Used)
	
#define STK_CALIB    (*((volatile unsigned long*) 0xE000E01C))					/*  SysTick Calibration Value Register is a read-only register
																																						that contains the number of pulses for a period of 10ms.
																																						We can use this Register to scale your value for the 
																																						Reload register, to generate an event at a timed interval,
																																						for example 1ms (Not Used)*/
#define GPIOB_CRL    (*((volatile unsigned long*) 0x40010C00))
#define GPIOB_BSRR   (*((volatile unsigned long*) 0x40010C10))		
	

volatile int timer = 0;					 /*Volatile keyword tells the compiler that the value of the variable "timer" may 
																	 change at any time-without any action being taken by the 
																	 code i.e. from outside world (in this case, Interrupt Routine)*/

void init()
{		
	STK_LOAD = (0x00004198);			// Value in Load register (72hz/1000)ms	
	
	// LED INIT
	GPIOB_CRL &= 0xFFFFFF0F;
	GPIOB_CRL |= 0x00000020;
}

void LED_On(void)
{
	GPIOB_BSRR = 0x00000002;
}

void LED_Off(void)
{
	GPIOB_BSRR = 0x00020000;
}

void SysTick_Handler(void)			// Interrupt service routines
{
	++timer;	
}



void Delay(int ms)
{
	//Starting SysTick timer
	STK_CTRL = 0x00000007;				// Use Processor Clock, asserts the SysTick exception request, SysTick Start				
	
	while(timer < ms);		
	
	//Stoping SysTick timer
	STK_CTRL = 0x00000000;
	timer = 0;
}		
	
int main(void)
{
	init();
	
	while(1)
	{
		LED_On();
		Delay(1000);								// time in milliseconds
		LED_Off();
		Delay(1000);
	}
	
	return (1);
}

void SystemInit(void)
{
}