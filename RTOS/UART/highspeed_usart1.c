#define RCC_APB2ENR	 (*((volatile unsigned long*) 0x40021018))			// USART1 and GPIO(port A) Clock enable
#define GPIOA_CRH		 (*((volatile unsigned long*) 0x40010804))
	
#define USART1_SR  	 (*((volatile unsigned long*) 0x40013800))
#define USART1_DR  	 (*((volatile unsigned long*) 0x40013804))
#define USART1_BRR   (*((volatile unsigned long*) 0x40013808))
#define USART1_CR1   (*((volatile unsigned long*) 0x4001380C))
#define USART1_CR2   (*((volatile unsigned long*) 0x40013810))
	
#define APB2_FREQ 72000000

short _parityError_1 = 0;

void USART1_Send(char* data)
{	
	unsigned int length = 0;
	while(data[length] != '\0')
		++length;		

	int i = 0;
	while(i < length)
	{
		USART1_DR = data[i];
		while((USART1_SR & 0x00000080) == 0);
		++i;
	}
}

char USART1_Receive()
{
	if((USART1_SR & 0x00000020) == 0)
		return 0;
	
	if((USART1_SR &	0x00000001) != 0)	// check if parity error occured
	{
		_parityError_1 = 1;
		return USART1_DR;
	}
	
	_parityError_1 = 0;
	return USART1_DR;
}

void USART1_Finish()
{
	while((USART1_SR & 0x00000040) == 0);
	USART1_CR1 &= 0x00000000;
}


//You don't need to configure the GPIOS, USART1_Init function does that for you.
//port A pin 9(TX)
//port A pin 10(RX)
int USART1_Init(int baudrate, short parity, short stop)
{
	if(baudrate <= 0 || parity < 0 || parity > 2 || stop < 1 || stop > 2)
		return 0;
	
	RCC_APB2ENR |= 0x00004004;				// enable Port A, enable usart1 clock
	
	GPIOA_CRH &= 0xFFFFF00F;
	GPIOA_CRH |= 0x000004A0;
	
	
	USART1_CR1 &=	0xFFFFC9FF;					// resetting the parity bit to 0(which is no parity) and "M bit" to 0(which is 8 bit data)
	
	if(parity == 1)										// even parity
		USART1_CR1 |=	0x00000400;				// enabling the parity bit first and setting it to even parity by writing 0 to bit 9
	else if(parity == 2)
		USART1_CR1 |=	0x00000600;				// odd parity
	
	
	USART1_CR2 &=	0xFFFFCFFF;					// 1 stop bit (write 00 to bits 13:12)
	if(stop == 2)
		USART1_CR2 |=	0x00002000;				// 2 stop bit (write 10 to bits 13:12)

	// setting the baudrate
	USART1_BRR = APB2_FREQ /(baudrate * 16.0);				// Formula for baudrate is baud = Fck / 16 * baudrate, sampling 16 times the baudrate
	
	USART1_CR1 |=	0x00002000;					// enable UART (write 1 to bit 13[UE (Uart Enable)])	
  
	// enable Transmiter (write 1 to bit 3[TE (Transmiter Enable)])
	// enable Receiver (write 1 to bit 2[RE (Receiver Enable)])
	USART1_CR1 |=	0x0000000C;
	USART1_Send("LINK OK:\r\n");
	
	return 1;
}

int main(void)
{
	if(USART1_Init(9600, 1, 1) == 0)
	{
		__asm(
			"MOV R1, 0xA\n\t"
			"MOV R2, 0xB\n\t"
		);	
	}
	USART1_Finish();
	
	while(1)
	{}
	
	return 1;
}

void SystemInit(void)
{}