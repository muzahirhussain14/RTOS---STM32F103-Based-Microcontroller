#define RCC_APB1ENR  (*((volatile unsigned long*) 0x4002101C))		// usart 3 clock enable
#define RCC_APB2ENR  (*((volatile unsigned long*) 0x40021018))		// usart 3 GPIO(port B) enable

#define GPIOB_CRH  	 (*((volatile unsigned long*) 0x40010C04))		// GPIO Port B

#define USART3_SR  	 (*((volatile unsigned long*) 0x40004800))
#define USART3_DR  	 (*((volatile unsigned long*) 0x40004804))
#define USART3_BRR   (*((volatile unsigned long*) 0x40004808))
#define USART3_CR1   (*((volatile unsigned long*) 0x4000480C))
#define USART3_CR2   (*((volatile unsigned long*) 0x40004810))

#define APB1_FREQ 36000000
short _parityError_3 = 0;

void USART3_Send(char* data)
{	
	unsigned int length = 0;
	while(data[length] != '\0')
		++length;		

	int i = 0;
	while(i < length)
	{
		USART3_DR = data[i];
		while((USART3_SR & 0x00000080) == 0);
		++i;
	}
}

char USART3_Receive()
{
	if((USART3_SR & 0x00000020) == 0)
		return 0;
	
	if((USART3_SR &	0x00000001) != 0)	// check if paroity error occured
	{
		_parityError_3 = 1;
		return USART3_DR;
	}
	
	_parityError_3 = 0;
	return USART3_DR;
}

void USART3_Finish()
{
	while((USART3_SR & 0x00000040) == 0);
	USART3_CR1 &=	0x00000000;
}


//You don't need to configure the GPIOS, USART1_Init function does that for you.

//port B pin 10(TX)
//port B pin 11(RX)
int USART3_Init(int baudrate, short parity, short stop)
{
	if(baudrate <= 0 || parity < 0 || parity > 2 || stop < 1 || stop > 2)
		return 0;
	
	//Configure Port B pin 10, 11
	RCC_APB2ENR |= 0x00000008;					// enable clock for port b
	
	GPIOB_CRH &= 0xFFFF00FF;						// resetting pin 10, 11
	GPIOB_CRH |= 0x00004A00;	
	
	
	// configuring USART 3 registers
	RCC_APB1ENR |= 0x00040000;
	
	USART3_CR1 &=	0xFFFFC9FF;					// resetting the parity bit to 0(which is no parity) and "M bit" to 0(which is 8 bit data)
	
	if(parity == 1)										// even parity
		USART3_CR1 |=	0x00000400;				// enabling the parity bit first and setting it to even parity by writing 0 to bit 9
	else if(parity == 2)
		USART3_CR1 |=	0x00000600;				// odd parity
	
	
	USART3_CR2 &=	0xFFFFCFFF;					// 1 stop bit (write 00 to bits 13:12)
	if(stop == 2)
		USART3_CR2 |=	0x00002000;				// 2 stop bit (write 10 to bits 13:12)

	// setting the baudrate
	USART3_BRR = APB1_FREQ /(baudrate * 16.0);				// Formula for baudrate is baud = Fck / 16 * baudrate, sampling 16 times the baudrate
	
	USART3_CR1 |=	0x00002000;					// enable UART (write 1 to bit 13[UE (Uart Enable)])	
  
	// enable Transmiter (write 1 to bit 3[TE (Transmiter Enable)])
	// enable Receiver (write 1 to bit 2[RE (Receiver Enable)])
	USART3_CR1 |=	0x0000000C;
	USART3_Send("LINK OK:\r\n");
	
	return 1;
}


/*
int main(void)
{	
	if(USART3_Init(7590, 2, 2) == 0)
	{
		__asm(
			"MOV R1, 0xA\n\t"
			"MOV R2, 0xB\n\t"
		);	
	}
	
	USART3_Send("AT+CIPSEND\n\r");
		
	volatile char data1 = USART2_Receive();
	
	char* data = "OK";
	USART3_Send(data);
	
	USART3_Send("End");
	USART3_Finish();
	
	while(1)
	{}

	return 1;
}

void SystemInit(void)
{}*/