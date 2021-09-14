#include "adc.h"

/*
Note:
1) Only ADC 1 is supported.
2) Fixed Resolution set to 12-bits.
3) Regular channels are supported only, no Injected channels.
4) Only First Five External Channels(Channel 0-4) is supported.
5) Use PA0, PA1, PA2, PA3, PA4 for channels 0, 1, 2, 3, 4 respectively. Only one channel can be configured and used at a time.
6) Only single conversion mode and continous mode.
7) After each conversion, read the result from conversionResult variable. -1 indicates that conversion didnot take place.
*/


volatile int conversionResult = -1;


void ADC1_2_IRQHandler(void) 		// ADC Interrupt service routines
{ 	
	conversionResult = ADC1_DR;							// reading data from dr register, EOC will be automaically reset
	
	// reset STRT Bit
	ADC1_SR &= 0xFFFFFFEF;
}


/*
Description:
	Call this function before using ADC 1. You don't need to configure the gpios, this function does that for you.
	Only first 5 [0-5] channels are supported.

Parameters:
	-> mode: 0,1
	   0: Single Conversion Mode (For Regular channel only)
	   1: Continous Mode	(For Regular Channel Only)
  -> alignment: 0,1
     0: right
     1: left
*/

int ADC_Init(short mode, short alignment)		//return 0 if unsuccessful, return 1 for success.
{
	if((mode != 0 && mode != 1) || (alignment != 0 && alignment != 1))
		return 0;
	
	// enable ADC Interrupts
	NVIC_SETENA_0 |= 0x00040000;		// Enable ADC_1 Interrupt
	
	RCC_APB2ENR |= 0x00000004;			// Enable port A clock
	RCC_APB2ENR |= 0x00000200;			// enable clock for ADC_1
	
	ADC1_CR2 &= 0x00000000;				// resetting controll registers
	ADC1_CR1 &= 0x00000000;
	
	ADC1_CR1 |= 0x00000020;				// EOC interrupt enable
	
	if(mode == 1)									// continous mode
		ADC1_CR2 |= 0x00000002;
	if(alignment == 1)
		ADC1_CR2 |= 0x00000800;
	
	// Enabling ADC
	
	/* This bit is set and cleared by software. If this bit holds a value of zero and a 1 is written to it
     then it wakes up the ADC from Power Down state.
     Conversion starts when this bit holds a value of 1 and a 1 is written to it.*/
	
	ADC1_CR2 |= 0x00000001;			// Waking up from power Down State

	// Resetting calibration Registers
	ADC1_CR2 |= 0x00000008;
	while((ADC1_CR2 & 0x00000008) != 0);
	
  // Perfroming Calibration
	ADC1_CR2 |= 0x00000004;
	while((ADC1_CR2 & 0x00000004) != 0);
	
	return 1;
}

/*
Description:
	Call this function after Init to configure the channels.

Parameters:
	-> channelNumber: 0,1,2,3,4 (The channel you want to configure)
	-> sampleTime: 0,1,2,3,4,5,6,7
		 0: 1.5 cycles
     1: 7.5 cycles
     2: 13.5 cycles
     3: 28.5 cycles
     4: 41.5 cycles
     5: 55.5 cycles
     6: 71.5 cycles
     7: 239.5 cycles
*/
void ADC_ChannelConfig(short channelNumber, short sampleTime)
{
	//Configuring the gpios to analog input
	if(channelNumber == 0)
		GPIOA_CRL &= 0xFFFFFFF0;
	else if(channelNumber == 1)
		GPIOA_CRL &= 0xFFFFFF0F;
	else if(channelNumber == 2)
		GPIOA_CRL &= 0xFFFFF0FF;
	else if(channelNumber == 3)
		GPIOA_CRL &= 0xFFFF0FFF;
	else if(channelNumber == 4)
		GPIOA_CRL &= 0xFFF0FFFF;	
	
	//selecting channel
	ADC1_SQR3 |= channelNumber;
	
	//setting sample time for this channel
	int temp = sampleTime;
	temp = (temp << channelNumber * 3);
	
	ADC1_SMPTR2 |= temp;
}


/*
Description:
  After calling this function, read the result from variable "conversionResult".
*/

void ADC_StartConversion()
{
	ADC1_CR2 |= 0x00000001;				// enable adc to start conversion now
}
 	
void ADC_Finish(short channel)
{
	RCC_APB2ENR &= 0xFFFFDFF;			// disable clock for ADC_1
	
	ADC1_CR2 &= 0x00000000;				// resetting controll registers
	ADC1_CR1 &= 0x00000000;
}