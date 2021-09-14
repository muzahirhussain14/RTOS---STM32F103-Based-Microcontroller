#define RCC_APB1ENR  (*((volatile unsigned long*) 0x4002101C))    	// clock enable for usart 2,3 uart 4, 5 -> 36 MHZ clock (36000000 cycles)
#define RCC_APB2ENR	 (*((volatile unsigned long*) 0x40021018))			// GPIO(port A) Clock enable

#define USART2_SR  	 (*((volatile unsigned long*) 0x40004400))    	// usart status register
#define USART2_DR  	 (*((volatile unsigned long*) 0x40004404))    	// usart data register
#define USART2_BRR   (*((volatile unsigned long*) 0x40004408))    	// baudrate register
#define USART2_CR1   (*((volatile unsigned long*) 0x4000440C))    	// controll register_1
#define USART2_CR2   (*((volatile unsigned long*) 0x40004410))    	// controll register_2

#define GPIOA_CRL		 (*((volatile unsigned long*) 0x40010800))
	
#define APB1_FREQ 36000000
short _parityError_2 = 0;

void USART2_Send(char* data)				// returns null if there is no data or parity error occurs
{	
	unsigned int length = 0;
	while(data[length] != '\0')
		++length;												// calculating the length of string		

	int i = 0;
	while(i < length)
	{
		USART2_DR = data[i];
		while((USART2_SR & 0x00000080) == 0);	// if the TXE (bit 7) in USART_SR is 0 this means data is not transmitted to shief register from data register yet
		++i;
	}
}

char USART2_Receive()			// reads and returns the character if available else it returns null
{	
	if((USART2_SR & 0x00000020) == 0)		// no data available
		return 0;
		
	if((USART2_SR &	0x00000001) != 0)		// check if paroity error occured
	{
		_parityError_2 = 1;
		return USART2_DR;
	}			
	
	_parityError_2 = 0;
	return USART2_DR;									// read and return data
}

void USART2_Finish()								// Call this function at the end to disable USART2
{
	while((USART2_SR & 0x00000040) == 0);	// if the TC (bit 6) in USART_SR is 0 this means data is not transmitted yet
	USART2_CR1 &=	0x00000000;					// diable USART2 (write 0 to bit 13[UE (Uart Enable)])
}


//You don't need to configure the GPIOS, USART1_Init function does that for you.
//USART2 TX-> PA2
//USART2 RX-> PA3

// parity -> 0 no parity, 1 even, 2 odd
// stop -> 1 (1 stop bit), 2(2 stop bit)
int USART2_Init(int baudrate, short parity, short stop)    // returns 0 for unsuccessfull else returns 1
{
	if(baudrate <= 0 || parity < 0 || parity > 2 || stop < 1 || stop > 2)
		return 0;
	
  RCC_APB2ENR |= 0x00000004;				// enable Port A clock because port A pin 2,3 will be used as transmiter and receiver resp.
	
	// PORT A pin 2, 3 configuration
	GPIOA_CRL &= 0xFFFF00FF;
	GPIOA_CRL |= 0x00004A00;					// pin 2(tx) alternate function push-pull and pin 3(rx) Input floating	
	
	RCC_APB1ENR |= 0x00020000;				// enable clock for usart 2(write 1 to bit 17)
	
	
	USART2_CR1 &=	0xFFFFC9FF;					// resetting the parity bit to 0(which is no parity) and "M bit" to 0(which is 8 bit data)
	
	if(parity == 1)										// even parity
		USART2_CR1 |=	0x00000400;				// enabling the parity bit first and setting it to even parity by writing 0 to bit 9
	else if(parity == 2)
		USART2_CR1 |=	0x00000600;				// odd parity
	
	
	USART2_CR2 &=	0xFFFFCFFF;					// 1 stop bit (write 00 to bits 13:12)
	if(stop == 2)
		USART2_CR2 |=	0x00002000;				// 2 stop bit (write 10 to bits 13:12)

	// setting the baudrate
	USART2_BRR = APB1_FREQ /(baudrate * 16.0);				// Formula for baudrate is baud = Fck / 16 * baudrate, sampling 16 times the baudrate
	
	USART2_CR1 |=	0x00002000;					// enable UART (write 1 to bit 13[UE (Uart Enable)])	
  
	// enable Transmiter (write 1 to bit 3[TE (Transmiter Enable)])
	// enable Receiver (write 1 to bit 2[RE (Receiver Enable)])
	USART2_CR1 |=	0x0000000C;
	USART2_Send("LINK OK:\r\n");
	
	return 1;
}
/*
int main(void)
{	
	if(USART2_Init(115200, 2, 1) == 0)
	{
		__asm(
			"MOV R1, 0xA\n\t"
			"MOV R2, 0xB\n\t"
		);	
	}
	
	//USART2_Send("AT+CIPSEND\n\r");
		
	char data1 = USART2_Receive();
	
//	char* data = "OK";
//	USART2_Send(data);
	
	USART2_Send("End");
  USART2_Finish();
	
	while(1)
	{
	}
	return (1);
}

void SystemInit(void)
{
}*/