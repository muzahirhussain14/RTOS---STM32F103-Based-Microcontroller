#define RCC_APB2ENR  (*((volatile unsigned long*) 0x40021018))
#define RCC_APB1ENR  (*((volatile unsigned long*) 0x4002101C))

//TIM2 has 8MHZ clock 
#define TIM2_CR1     (*((volatile unsigned long*) 0x40000000))				// Controll Register 1
#define TIM2_CNT     (*((volatile unsigned long*) 0x40000024))				// Counter will count up from 0 to Auto-reload register
#define TIM2_PSC     (*((volatile unsigned long*) 0x40000028))				// The Prescaler
#define TIM2_ARR     (*((volatile unsigned long*) 0x4000002C))				// Auto Reload register
#define TIM2_SR      (*((volatile unsigned long*) 0x40000010))
	
#define GPIOB_CRL    (*((volatile unsigned long*) 0x40010C00))
#define GPIOB_BSRR   (*((volatile unsigned long*) 0x40010C10))	

void Delay_1(const unsigned short PERIOD_IN_MILLISEC)
{
	RCC_APB1ENR |= 0x00000001;				// enable clock for tim2
	TIM2_CR1=0;
	TIM2_CNT=0;
	
	if(PERIOD_IN_MILLISEC > 0)
	{
		TIM2_PSC = 7999;								// 1ms -> it means that the frequency supply for the counter will be 1ms
		TIM2_ARR = PERIOD_IN_MILLISEC - 1;				// count from 0 to this value
	}
	TIM2_CR1 =0x001;									// enable Counter
	
	while((TIM2_SR & 0x00000001) == 0);
	TIM2_SR = 0;
	TIM2_CR1 =0x000;									// disable Counter
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
		//LED_Toggle();
		LED_On();
		Delay_1(1000);

		LED_Off();
		Delay_1(1000);
	}
	return (1);
}

void SystemInit(void)
{
}