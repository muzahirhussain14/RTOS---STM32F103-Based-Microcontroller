#define RCC_APB2ENR  (*((volatile unsigned long*) 0x40021018))
#define RCC_APB1ENR  (*((volatile unsigned long*) 0x4002101C))

//TIM3 has 8MHZ clock 
#define TIM3_CR1     (*((volatile unsigned long*) 0x40000400))				// Controll Register 1
#define TIM3_CNT     (*((volatile unsigned long*) 0x40000424))				// Counter will count up from 0 to Auto-reload register
#define TIM3_PSC     (*((volatile unsigned long*) 0x40000428))				// The Prescaler
#define TIM3_ARR     (*((volatile unsigned long*) 0x4000042C))				// Auto Reload register
#define TIM3_SR      (*((volatile unsigned long*) 0x40000410))
	
#define GPIOB_CRL    (*((volatile unsigned long*) 0x40010C00))
#define GPIOB_BSRR   (*((volatile unsigned long*) 0x40010C10))	

void Delay_2(const unsigned short PERIOD_IN_MILLISEC)
{
	RCC_APB1ENR |= 0x00000002;				// enable clock for TIM3
	TIM3_CR1=0;
	TIM3_CNT=0;
	
	if(PERIOD_IN_MILLISEC > 0)
	{
		TIM3_PSC = 7999;										// 1ms -> it means that the frequency supply for the counter will be 1ms
		TIM3_ARR = PERIOD_IN_MILLISEC - 1;	// count from 0 to this value
	}
	TIM3_CR1 =0x001;									// enable Counter
	
	while((TIM3_SR & 0x00000001) == 0);
	TIM3_SR = 0;
	TIM3_CR1 =0x000;									// disable Counter
}

void LED_Init(void)
{
	RCC_APB2ENR |= 0x00000004; GPIOB_CRL &= 0xFFFFFF0F;
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

int main(void)
{
	LED_Init();
	
	while(1)
	{
		LED_On();
		Delay_2(200);

		LED_Off();
		Delay_2(200);
	}
	return (1);
}

void SystemInit(void)
{
}