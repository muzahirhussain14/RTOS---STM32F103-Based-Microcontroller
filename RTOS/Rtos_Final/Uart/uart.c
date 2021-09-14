#include "uart.h"

// Only ASynchronous Mode is Supported.

//................................USART 1 (High Speed)...............................

/*
Note:
1) Uart1 runs at 48Mhz PLL clock source(provided by HCI) instead of usual 8Mhz HCI. Using UART1 might effect other peripherals that are configured to run with default 8Mhz HSI clock source.
   Don't use UART1 along with other UARTs, because other UARTs run at 8Mhz clock.
2) You don't need to configure the GPIOS, UART1_Init function does that for you.
3) port A pin 9(TX)
   port A pin 10(RX)
4) Databits = 8 (only)
*/

short _parityError_1 = 0;						// if a parity error occurs _parityError_1 is set to 1

void UART1_Send(char* data)
{	
	unsigned int length = 0;
	while(data[length] != '\0')
		++length;		

	int i = 0;
	while(i < length)
	{
		while((USART1_SR & 0x00000080) == 0);
		USART1_DR = data[i];
		++i;
	}
	while((USART1_SR & 0x00000040) == 0);	// wait while TC bit is equal to 1
}


char UART1_Receive()
{
	if((USART1_SR & 0x00000020) == 0)	// if no data available return null
		return '\0';
	
	if((USART1_SR &	0x00000001) != 0)	// check if parity error occured
	{
		_parityError_1 = 1;
		return USART1_DR;
	}
	
	_parityError_1 = 0;
	return USART1_DR;
}

/*
 Calling UART1_Finish will reset the clock source back to 8 Mhz HSI.
*/
void UART1_Finish()
{
	USART1_CR1 &= 0x00000000;
	USART1_CR2 &= 0x00000000;
	RCC_APB2ENR &= 0xFFFFBFFF;				// disable clock Port A

	// reset to HSI 8 Mhz Clock source	
	RCC_CFGR &= 0xFFFFFFFC;   				// use HSI as system clock
	RCC_CR &= 0xFEFFFFFF;					// Pll Off
}


/*
Discription:
	Call this function before using UART_1.

Parameters:
	-> baudrate = Enter the required baud rate e.g. 9600, 112500 etc

	-> parity = 0,1,2
		 0: No Parity
		 1: Even Parity
		 2: Odd Parity
	
  -> stop = 1,2
		1: 1 Stop Bit
		2: 2 Stop Bit
*/
int UART1_Init(int baudrate, short parity, short stop)
{
	if(baudrate <= 0 || parity < 0 || parity > 2 || stop < 1 || stop > 2)
		return 0;
	
	/* Changing Clock Source To 48MHz PLL*/
	
	// PLL multiplies by factor of 12          
    RCC_CFGR   |= 0x00280000;
		
	// HSI frequency divided is divided by 2 by default before entering PLL
    // activate pll
	RCC_CR |= 0x01000000;
	while((RCC_CR & 0x02000000) == 0); // wait until PLL is not locked
	RCC_CFGR |= 0x00000002;   // use PLL as system clock
	
	RCC_APB2ENR |= 0x00004004;				// enable Port A, enable usart1 clock
	
	GPIOA_CRH &= 0xFFFFF00F;
	GPIOA_CRH |= 0x000004B0;
	
	
	USART1_CR1 &=	0x00000000;					// resetting the parity bit to 0(which is no parity) and "M bit" to 0(which is 8 bit data)
	
	if(parity == 1)										// even parity
		USART1_CR1 |=	0x00000400;				// enabling the parity bit first and setting it to even parity by writing 0 to bit 9
	else if(parity == 2)
		USART1_CR1 |=	0x00000600;				// odd parity
	
	
	USART1_CR2 &=	0x00000000;					// 1 stop bit (write 00 to bits 13:12)
	if(stop == 2)
		USART1_CR2 |=	0x00002000;				// 2 stop bit (write 10 to bits 13:12)

	// setting the baudrate
	USART1_BRR = (APB2_FREQ - 24000000)/baudrate;			// the clock runs at 48MHZ
	
	USART1_CR1 |=	0x00002000;					// enable UART (write 1 to bit 13[UE (Uart Enable)])	
  
	// enable Transmiter (write 1 to bit 3[TE (Transmiter Enable)])
	// enable Receiver (write 1 to bit 2[RE (Receiver Enable)])
	USART1_CR1 |=	0x0000000C;
	UART1_Send("LINK OK:\r\n");
	
	return 1;
}






//..............................USART_2...................................
/*
Note:
1) Runs at default 8Mhz HSI
2) You don't need to configure the GPIOS, UART2_Init function does that for you.
3) USART2 TX-> PA2
   USART2 RX-> PA3
   Also note that by default PA2 and PA3 pins (i.e. D1 and D0 on Arduino connector) on STM32 Nucleo are connected to the ST-LINK/V2-1 interface, and
   therefore not usable without adding and removing some bridges first.
4) Databits = 8 (only)
*/


short _parityError_2 = 0;		// if a parity error occurs in USART_2, _parityError_2 is set to 1

void UART2_Send(char* data)				// returns null if there is no data or parity error occurs
{	
	unsigned int length = 0;
	while(data[length] != '\0')
		++length;												// calculating the length of string		

	int i = 0;
	while(i < length)
	{
		while((USART2_SR & 0x00000080) == 0);	// if the TXE (bit 7) in USART_SR is 0 this means data is not transmitted to shief register from data register yet		
		USART2_DR = data[i];
		++i;
	}
	while((USART2_SR & 0x00000040) == 0);	// if the TC (bit 6) in USART_SR is 0 this means data is not transmitted yet
}

char UART2_Receive()			// reads and returns the character if available else it returns null
{	
	if((USART2_SR & 0x00000020) == 0)		// no data available
		return '\0';
		
	if((USART2_SR &	0x00000001) != 0)		// check if paroity error occured
	{
		_parityError_2 = 1;
		return USART2_DR;
	}			
	
	_parityError_2 = 0;
	return USART2_DR;									// read and return data
}

void UART2_Finish()								// Call this function at the end to disable USART2
{
	USART2_CR1 &=	0x00000000;					// diable USART2 (write 0 to bit 13[UE (Uart Enable)])
	USART2_CR2 &=	0x00000000;
	RCC_APB1ENR &= 0xFFFDFFFF;					// disable clock for usart 2(write 0 to bit 17)
}



/*
Discription:
	Call this function before using UART_2.

Parameters:
	-> baudrate = Enter the required baud rate e.g. 9600, 112500 etc

	-> parity = 0,1, 2
		 0: No Parity
		 1: Even Parity
		 2: Odd Parity
	
  -> stop = 1,2
		1: 1 Stop Bit
		2: 2 Stop Bit
*/
int UART2_Init(int baudrate, short parity, short stop)    // returns 0 for unsuccessfull else returns 1
{
	if(baudrate <= 0 || parity < 0 || parity > 2 || stop < 1 || stop > 2)
		return 0;
	
	RCC_CFGR &= 0xFFFFFFFC;  					// use 8MHZ HSI as system clock
	RCC_APB2ENR |= 0x00000004;				// enable Port A clock because port A pin 2,3 will be used as transmiter and receiver resp.
	
	// PORT A pin 2, 3 configuration
	GPIOA_CRL &= 0xFFFF00FF;
	GPIOA_CRL |= 0x00004B00;					// pin 2(tx) alternate function push-pull and pin 3(rx) Input floating	
	
	RCC_APB1ENR |= 0x00020000;				// enable clock for usart 2(write 1 to bit 17)
	
	
	USART2_CR1 &=	0x00000000;					// resetting the parity bit to 0(which is no parity) and "M bit" to 0(which is 8 bit data)
	
	if(parity == 1)										// even parity
		USART2_CR1 |=	0x00000400;				// enabling the parity bit first and setting it to even parity by writing 0 to bit 9
	else if(parity == 2)
		USART2_CR1 |=	0x00000600;				// odd parity
	
	
	USART2_CR2 &=	0x00000000;					// 1 stop bit (write 00 to bits 13:12)
	if(stop == 2)
		USART2_CR2 |=	0x00002000;				// 2 stop bit (write 10 to bits 13:12)

	// setting the baudrate
	USART2_BRR = 8000000 / baudrate;	// Formula for baudrate is baud = Fck / 16 * baudrate, sampling 16 times the baudrate
	
	USART2_CR1 |=	0x00002000;					// enable UART (write 1 to bit 13[UE (Uart Enable)])	
  
	// enable Transmiter (write 1 to bit 3[TE (Transmiter Enable)])
	// enable Receiver (write 1 to bit 2[RE (Receiver Enable)])
	USART2_CR1 |=	0x0000000C;
	UART2_Send("LINK OK:\r\n");
	
	return 1;
}

//.................................. USART_3...................................
/*Note
1) Runs at default 8Mhz HSI
2) You don't need to configure the GPIOS, UART3_Init function does that for you.
3) port B pin 10(TX)
   port B pin 11(RX)
4) Databits = 8 (only)
*/

short _parityError_3 = 0;		// if a parity error occurs in USART_3, _parityError_3 is set to 1

void UART3_Send(char* data)
{	
	unsigned int length = 0;
	while(data[length] != '\0')
		++length;		

	int i = 0;
	while(i < length)
	{
		while((USART3_SR & 0x00000080) == 0);
		USART3_DR = data[i];
		++i;
	}
	while((USART3_SR & 0x00000040) == 0);
}

char UART3_Receive()
{
	if((USART3_SR & 0x00000020) == 0)
		return '\0';
	
	if((USART3_SR &	0x00000001) != 0)	// check if paroity error occured
	{
		_parityError_3 = 1;
		return USART3_DR;
	}
	
	_parityError_3 = 0;
	return USART3_DR;
}

void UART3_Finish()
{
	RCC_APB1ENR |= 0xFFFBFFFF;		// disable USART3 Clock
	USART3_CR1 &=	0x00000000;
	USART3_CR2 &=	0x00000000;
}


/*
Discription:
	Call this function before using UART_3.

Parameters:
	-> baudrate = Enter the required baud rate e.g. 9600, 112500 etc

	-> parity = 0,1,2
		 0: No Parity
		 1: Even Parity
		 2: Odd Parity
	
	-> stop = 1,2
		1: 1 Stop Bit
		2: 2 Stop Bit
*/
int UART3_Init(int baudrate, short parity, short stop)
{
	if(baudrate <= 0 || parity < 0 || parity > 2 || stop < 1 || stop > 2)
		return 0;

    RCC_CFGR &= 0xFFFFFFFC;   				// use 8MHZ HSI as system clock
		
	//Configure Port B pin 10, 11
	RCC_APB2ENR |= 0x00000008;					// enable clock for port b
	
	GPIOB_CRH &= 0xFFFF00FF;					// resetting pin 10, 11
	GPIOB_CRH |= 0x00004B00;	
	
	
	// configuring USART 3 registers
	RCC_APB1ENR |= 0x00040000;
	
	USART3_CR1 &=	0x00000000;					// resetting the parity bit to 0(which is no parity) and "M bit" to 0(which is 8 bit data)
	
	if(parity == 1)								// even parity
		USART3_CR1 |=	0x00000400;				// enabling the parity bit first and setting it to even parity by writing 0 to bit 9
	else if(parity == 2)
		USART3_CR1 |=	0x00000600;				// odd parity
	
	
	USART3_CR2 &=	0x00000000;					// 1 stop bit (write 00 to bits 13:12)
	if(stop == 2)
		USART3_CR2 |=	0x00002000;				// 2 stop bit (write 10 to bits 13:12)

	// setting the baudrate
	USART3_BRR = 8000000 / baudrate;				// Formula for baudrate is baud = Fck / 16 * baudrate, sampling 16 times the baudrate
	
	USART3_CR1 |=	0x00002000;					// enable UART (write 1 to bit 13[UE (Uart Enable)])	
  
	// enable Transmiter (write 1 to bit 3[TE (Transmiter Enable)])
	// enable Receiver (write 1 to bit 2[RE (Receiver Enable)])
	USART3_CR1 |=	0x0000000C;
	UART3_Send("LINK OK:\r\n");
	
	return 1;
}