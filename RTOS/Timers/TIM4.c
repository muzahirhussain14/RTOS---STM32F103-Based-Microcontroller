#define RCC_APB2ENR  (*((volatile unsigned long*) 0x40021018))
#define RCC_APB1ENR  (*((volatile unsigned long*) 0x4002101C))

//TIM4 has 8MHZ clock 
#define TIM4_CR1     (*((volatile unsigned long*) 0x40000800))				// Controll Register 1
#define TIM4_CNT     (*((volatile unsigned long*) 0x40000824))				// Counter will count up from 0 to Auto-reload register
#define TIM4_PSC     (*((volatile unsigned long*) 0x40000828))				// The Prescaler
#define TIM4_ARR     (*((volatile unsigned long*) 0x4000082C))				// Auto Reload register
#define TIM4_SR      (*((volatile unsigned long*) 0x40000810))
	
#define GPIOB_CRL    (*((volatile unsigned long*) 0x40010C00))
#define GPIOB_BSRR   (*((volatile unsigned long*) 0x40010C10))	

void Delay_3(const unsigned short PERIOD_IN_MILLISEC)
{
	RCC_APB1ENR |= 0x00000004;				// enable clock for TIM4
	TIM4_CR1=0;
	TIM4_CNT=0;
	
	if(PERIOD_IN_MILLISEC > 0)
	{
		TIM4_PSC = 7999;										// 1ms -> it means that the frequency supply for the counter will be 1ms
		TIM4_ARR = PERIOD_IN_MILLISEC - 1;	// count from 0 to this value
	}
	TIM4_CR1 =0x001;									// enable Counter
	
	while((TIM4_SR & 0x00000001) == 0);
	TIM4_SR = 0;
	TIM4_CR1 =0x000;									// disable Counter
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
		Delay_3(200);

		LED_Off();
		Delay_3(200);
	}
	return (1);
}

void SystemInit(void)
{
}