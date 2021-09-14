/*
NOTE:
These timers are configured to run at 8Mhz and 48Mhz(HSI with PLL) clock frequncies. Other clock frequencies might cause problem.
*/

#include "timers.h"
//.......................................TIM2....................................

void Delay_1(const unsigned int PERIOD_IN_MILLISEC)
{
	RCC_APB1ENR |= 0x00000001;				// enable clock for tim2
	TIM2_CR1=0;
	TIM2_CNT=0;
	
	if(PERIOD_IN_MILLISEC > 0)
	{
		if((RCC_CFGR & 0x00000002) == 0x00000002)			//48 Mhz using HSI with PLL
			TIM2_PSC = 47999;								// 1ms -> it means that the frequency supply for the counter will be 1ms when clock source is 48mhz
		else					// 8Mhz
			TIM2_PSC = 7999;								// 1ms -> it means that the frequency supply for the counter will be 1ms when clk source is 8Mhz
		
		TIM2_ARR = PERIOD_IN_MILLISEC - 1;				// count from 0 to this value
	}
	TIM2_CR1 =0x001;									// enable Counter
	
	while((TIM2_SR & 0x00000001) == 0);
	TIM2_SR = 0;
	TIM2_CR1 =0x000;									// disable timer
}


//....................................TIM3.......................................

void Delay_2(const unsigned int PERIOD_IN_MILLISEC)
{
	RCC_APB1ENR |= 0x00000002;				// enable clock for TIM3
	TIM3_CR1=0;
	TIM3_CNT=0;
	
	if(PERIOD_IN_MILLISEC > 0)
	{
		if((RCC_CFGR & 0x00000002) == 0x00000002)			//48 Mhz using HSI with PLL
			TIM3_PSC = 47999;								// 1ms -> it means that the frequency supply for the counter will be 1ms when clock source is 48mhz
		else					// 8Mhz
			TIM3_PSC = 7999;								// 1ms -> it means that the frequency supply for the counter will be 1ms when clk source is 8Mhz
		
		TIM3_ARR = PERIOD_IN_MILLISEC - 1;				// count from 0 to this value
	}
	TIM3_CR1 =0x001;									// enable Counter
	
	while((TIM3_SR & 0x00000001) == 0);
	TIM3_SR = 0;
	TIM3_CR1 =0x000;									// disable timer
}

//.................................TIM4.................................

void Delay_3(const unsigned int PERIOD_IN_MILLISEC)
{
	RCC_APB1ENR |= 0x00000004;				// enable clock for TIM4
	TIM4_CR1=0;
	TIM4_CNT=0;
	
	if(PERIOD_IN_MILLISEC > 0)
	{
		if((RCC_CFGR & 0x00000002) == 0x00000002)			//48 Mhz using HSI with PLL
			TIM4_PSC = 47999;								// 1ms -> it means that the frequency supply for the counter will be 1ms when clock source is 48mhz
		else					// 8Mhz
			TIM4_PSC = 7999;								// 1ms -> it means that the frequency supply for the counter will be 1ms when clk source is 8Mhz
		
		TIM4_ARR = PERIOD_IN_MILLISEC - 1;				// count from 0 to this value
	}
	TIM4_CR1 =0x001;									// enable Counter
	
	while((TIM4_SR & 0x00000001) == 0);
	TIM4_SR = 0;
	TIM4_CR1 =0x000;									// disable timer
}