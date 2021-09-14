#include "gpio.h"

volatile unsigned long* GPIO_BASE = (volatile unsigned long*) 0x40010800;			// points to GPIOA_CRL

/*
Discription:
	Call this function to configure GPIO pins before using it.
  You can call this function multiple times if you want to configure multiple pins.

Parameters:
	-> port = A,B,C,D (all in Caps)
     
	-> mode = 0,1,2,3
		 0: Input Mode
		 1: Output mode, max speed 10 Mhz
		 2: Output mode, max speed 2 MHZ
     3: Output mode, max speed 50 MHZ
	
  -> Operation = 0,1,2,3
		 (In case of Input Mode)
		 0: Analog Mode
		 1: Floating Input(reset state)
		 2: Input with pull-up/pull-down
		 3: Reserved

		 (In case of Output Mode)
		 0: General Purpose output push-pull
		 1: General Purpose output  open-drain
		 2: Alternate function output push-pull
		 3: Alternate function output open-drain

	-> pinNumber: 0-15 (The number of the pin that you are willing to configure, 0 repersents first pin)
*/

void GPIO_Init(char port, short mode, short operation, short pinNumber)
{
	volatile unsigned int temp = (int)port - 65;
	temp = temp * 1024;							// 1024 is the size between one GPIO_CRL to next GPIO_CRL
	temp = temp/4;
	
	volatile unsigned long* GPIO_Register = GPIO_BASE + temp;			//Pointing to the GPIO_CRL Register of required port
	
	temp = 0x00000001;
	temp = temp << (((int)port - 65) + 2);
	RCC_APB2ENR = (RCC_APB2ENR | temp);						// enable Timer
	
	if(pinNumber > 7)
	{
		pinNumber -= 8;																							// bringing it in range 0-7
		GPIO_Register++;																						// Increment 4 bytes -> pointing to CRH Register
	}
	
	temp = 0xFFFFFFF0;
	
	if(pinNumber > 0)
	{
	// Initlizing GPIO with 0
	// -> x = (x << shieft) | (x >> (sizeOf(x) * NumberOfBitsInBytes - shieft))
	temp = (temp << (pinNumber * 4)) | (temp >> (4 * 8 - (pinNumber * 4)));		// round shiefting 0 to required position.
	}	
	*GPIO_Register = *GPIO_Register & temp;												// initilizing the bits by 0
	
	
	// Setting Mode
	temp = 0x00000000;
	temp = (temp | mode);
	if(pinNumber > 0) 																						// right shiefting 32-bits gives undefined behaviour
		temp = (temp << (pinNumber * 4)) | temp >> (4 * 8 - (pinNumber * 4));		// round shiefting to required position.
	*GPIO_Register = *GPIO_Register | temp;
	
	
	// Setting Operation
	temp = 0x00000000;
	temp = temp | ((operation * 3) + operation);
	if(pinNumber > 0) 
		temp = (temp << (pinNumber * 4)) | temp >> (4 * 8 - (pinNumber * 4));		// round shiefting to required position.
	*GPIO_Register = *GPIO_Register | temp;
}


/*
Discription:
	Call this function to enable a pin.

Parameters:
	-> port = A,B,C,D (all in Caps)

	-> pinNumber: 0-15 (The pin number that you want to enable)
*/
void GPIO_EnablePin(char port, short pinNumber)
{
  volatile unsigned int temp = (int)port - 65;
	temp = temp * 1024;							// 1024 is the size between one GPIO_CRL to next GPIO_CRL
	temp = temp/4;
	
	volatile unsigned long* GPIO_Register = GPIO_BASE + temp;			//Pointing to the GPIO_CRL Register of required port
	GPIO_Register += 4;							// adding 16 bytes to point to BSRR register
	
	temp = 0x00000001;
	temp = temp << pinNumber;
	
	*GPIO_Register = (*GPIO_Register | temp);
}


/*
Discription:
	Call this function to disable a pin.

Parameters:
	-> port = A,B,C,D (all in Caps)

	-> pinNumber: 0-15 (The pin number that you want to enable)
*/
void GPIO_DisablePin(char port, short pinNumber)
{
  volatile unsigned int temp = (int)port - 65;
	temp = temp * 1024;							// 1024 is the size between one GPIO_CRL to next GPIO_CRL
	temp = temp/4;
	
	volatile unsigned long* GPIO_Register = GPIO_BASE + temp;			//Pointing to the GPIO_CRL Register of required port
	GPIO_Register += 4;							// adding 16 bytes to point to BSRR register
	
	pinNumber = pinNumber + 16;			// disabling starts from 16 bit
	temp = 0x00000001;
	temp = temp << pinNumber;
	
	*GPIO_Register = (*GPIO_Register | temp);
}