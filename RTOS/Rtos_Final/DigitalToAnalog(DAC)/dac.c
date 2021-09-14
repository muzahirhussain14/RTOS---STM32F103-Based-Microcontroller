#include "dac.h"

/*
Note:
1) Only 12 bit resolution is supported.
2) Use PA4, PA5 for channels 1, 2 respectively.
3) This library doesnot support Dual Mode, DMA request, Noise Generation, Triangle-wave generation.
4) Only single mode is supported.
*/




/*
Description:
	Call this function to initlize DAC channel before using it. You you don't need to configure GPIO, this function does that for you.

Parameters:
	-> channel: 1,2 (The channel you want to configure)
*/

int DAC_Init(short channel)						// returns 1 on success, 0 on false...
{
	if(channel != 1 && channel != 2)
		return 0;
	
	RCC_APB1ENR |= 0x20000000;					// enable clock for DAC
	
	if(channel == 1){
		RCC_APB2ENR |= 0x00000004;			// Enable port A clock
		GPIOA_CRL &= 0xFFF0FFFF;				// Configure PA4 as analog Input
		
		DAC_CR &= 0x00000000;						// resetting controll register for dac
	    DAC_CR |=	0x00000039;						// setting trigger selection to software and enabling DAC
	}
	if(channel == 2)
	{
		RCC_APB2ENR |= 0x00000004;				// Enable port A clock
		GPIOA_CRL &= 0xFF0FFFFF;					// Configure PA5 as analog Input
		
		DAC_CR &= 0x00000000;							// resetting controll register for dac
		DAC_CR |= 0x00390000;             // setting trigger selection to software and enabling DAC
	}
	
	return 1;
}

/*
Description:
	Call this function to convert the information to analog.

Parameters:
	-> data: a short number to convert.
	-> alignment: 0,1
			0: 12-bit Right alignment
			1: 12-bit Left alignment

	-> channel: 1,2 (same channel that you passed in DAC_Init function)
			1: channel 1
			2: channel 2
*/
void DAC_Convert(unsigned int data, short alignment, short channel)
{
	if(channel == 1)					//channel 1
	{
		DAC_SWTRIGR |= 0x00000001;		//software trigger for channel 1 enabled to start conversion
		
		if(alignment == 0)			// 12-bit right align
			DAC_DHR12R1 = data;
		
		if(alignment == 1)			// 12-bit left align
			DAC_DHR12L1 = data;
	}
		
	if(channel == 2)					//channel 2
	{
		DAC_SWTRIGR |= 0x00000002;    //software trigger for channel 2 enabled to start conversion
		
		if(alignment == 0)			// 12-bit right align
			DAC_DHR12R2 = data;
		
		if(alignment == 1)			// 12-bit left align
			DAC_DHR12L2 = data;
	}
}

void DAC_Finish()
{
	DAC_CR &= 0x00000000;
	RCC_APB1ENR |= 0xDFFFFFFF;	// disable clock
}