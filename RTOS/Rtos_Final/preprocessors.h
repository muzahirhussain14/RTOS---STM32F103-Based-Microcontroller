#ifndef PREPROCESSORS_H_
#define PREPROCESSORS_H_


#define RCC_APB1ENR  (*((volatile unsigned long*) 0x4002101C))		// usart 3 clock enable
#define RCC_APB2ENR	 (*((volatile unsigned long*) 0x40021018))			// USART1 and GPIO(port A) Clock enable


//......................................UART Preprocessors..........................................
// USART_1 Registers...
#define USART1_SR  	 (*((volatile unsigned long*) 0x40013800))
#define USART1_DR  	 (*((volatile unsigned long*) 0x40013804))
#define USART1_BRR   (*((volatile unsigned long*) 0x40013808))
#define USART1_CR1   (*((volatile unsigned long*) 0x4001380C))
#define USART1_CR2   (*((volatile unsigned long*) 0x40013810))

#define RCC_CR 	 	 (*((volatile unsigned long*) 0x40021000))
#define RCC_CFGR 	 (*((volatile unsigned long*) 0x40021004))		//clock configuration register

#define GPIOA_CRH	 (*((volatile unsigned long*) 0x40010804))
	

// USART_2 Registers...
#define USART2_SR  	 (*((volatile unsigned long*) 0x40004400))    	// usart status register
#define USART2_DR  	 (*((volatile unsigned long*) 0x40004404))    	// usart data register
#define USART2_BRR   (*((volatile unsigned long*) 0x40004408))    	// baudrate register
#define USART2_CR1   (*((volatile unsigned long*) 0x4000440C))    	// controll register_1
#define USART2_CR2   (*((volatile unsigned long*) 0x40004410))    	// controll register_2

#define GPIOA_CRL		 (*((volatile unsigned long*) 0x40010800))

// USART_3 Registers...
#define USART3_SR  	 (*((volatile unsigned long*) 0x40004800))
#define USART3_DR  	 (*((volatile unsigned long*) 0x40004804))
#define USART3_BRR   (*((volatile unsigned long*) 0x40004808))
#define USART3_CR1   (*((volatile unsigned long*) 0x4000480C))
#define USART3_CR2   (*((volatile unsigned long*) 0x40004810))

#define GPIOB_CRH  	 (*((volatile unsigned long*) 0x40010C04))		// GPIO Port B

//..........................................SysTick Timer(Context Switching)......................................

#define STK_CTRL     (*((volatile unsigned long*) 0xE000E010))					// sysTick configuration register
#define STK_LOAD     (*((volatile unsigned long*) 0xE000E014))					// sysTick load value register
#define STK_VAL      (*((volatile unsigned long*) 0xE000E018))					// systick Current Value Register(Not Used)
	
#define STK_CALIB    (*((volatile unsigned long*) 0xE000E01C))					/*  SysTick Calibration Value Register is a read-only register
																																						that contains the number of pulses for a period of 10ms.
																																						We can use this Register to scale your value for the 
																																						Reload register, to generate an event at a timed interval,
																																						for example 1ms (Not Used)*/

#define ICSR   		    (*((volatile unsigned long*) 0xE000ED04))					/*  Interrupt Controll and State register
																						Provides: set-pending and clear-pending bits for the PendSV and SysTick exceptions
																						Used to set/ disable PendSV exception 
																						Bit 27: pendSv clear 
																						Bit 28: pendSv set
																						*/

			
//............................................ GENERAL PURPOSE TIMERS .........................................	

//TIM2
#define TIM2_CR1     (*((volatile unsigned long*) 0x40000000))				// Controll Register 1
#define TIM2_CNT     (*((volatile unsigned long*) 0x40000024))				// Counter will count up from 0 to Auto-reload register
#define TIM2_PSC     (*((volatile unsigned long*) 0x40000028))				// The Prescaler
#define TIM2_ARR     (*((volatile unsigned long*) 0x4000002C))				// Auto Reload register
#define TIM2_SR      (*((volatile unsigned long*) 0x40000010))


//TIM3 
#define TIM3_CR1     (*((volatile unsigned long*) 0x40000400))				// Controll Register 1
#define TIM3_CNT     (*((volatile unsigned long*) 0x40000424))				// Counter will count up from 0 to Auto-reload register
#define TIM3_PSC     (*((volatile unsigned long*) 0x40000428))				// The Prescaler
#define TIM3_ARR     (*((volatile unsigned long*) 0x4000042C))				// Auto Reload register
#define TIM3_SR      (*((volatile unsigned long*) 0x40000410))

//TIM4 
#define TIM4_CR1     (*((volatile unsigned long*) 0x40000800))				// Controll Register 1
#define TIM4_CNT     (*((volatile unsigned long*) 0x40000824))				// Counter will count up from 0 to Auto-reload register
#define TIM4_PSC     (*((volatile unsigned long*) 0x40000828))				// The Prescaler
#define TIM4_ARR     (*((volatile unsigned long*) 0x4000082C))				// Auto Reload register
#define TIM4_SR      (*((volatile unsigned long*) 0x40000810))


//.................................................... SPI .......................................................

#define SPI_CR1	(*((volatile unsigned long*) 0x40013000))		// SPI_1 Control Register 1
#define SPI_CR2	(*((volatile unsigned long*) 0x40013004))		// SPI_1 Control Register 2
#define SPI_SR	(*((volatile unsigned long*) 0x40013008))		// SPI_1 Status Register
#define SPI_DR	(*((volatile unsigned long*) 0x4001300C))		// SPI_1 Data Register


#define GPIOA_BSRR (*((volatile unsigned long*) 0x40010810))

#define GPIOB_CRL  (*((volatile unsigned long*) 0x40010C00))
#define GPIOB_BSRR (*((volatile unsigned long*) 0x40010C10))

#define GPIOC_CRL  (*((volatile unsigned long*) 0x40011000))
#define GPIOC_BSRR (*((volatile unsigned long*) 0x40011010))


//................................................... DAC ...........................................................

#define DAC_CR   	  (*((volatile unsigned long*) 0x40007400))
#define DAC_SWTRIGR   (*((volatile unsigned long*) 0x40007404))		//Software Trigger Register

#define DAC_DHR12R1   (*((volatile unsigned long*) 0x40007408))		//12 bit right aligned data-holding register channel_1
#define DAC_DHR12L1   (*((volatile unsigned long*) 0x4000740C))		//12 bit left aligned data-holding register channel_1

#define DAC_DHR12R2   (*((volatile unsigned long*) 0x40007414))		//12 bit right aligned data-holding register channel_2
#define DAC_DHR12L2   (*((volatile unsigned long*) 0x40007418))		//12 bit left aligned data-holding register channel_2

#define DAC_DOR1   	  (*((volatile unsigned long*) 0x4000742C))		//Data output register channel 1
#define DAC_DOR2   	  (*((volatile unsigned long*) 0x40007430))		//Data output register channel 2


//................................................... ADC .............................................................
	
#define ADC1_SR  	  (*((volatile unsigned long*) 0x40012400))				// ADC_1 Status Register
#define ADC1_CR1  	  (*((volatile unsigned long*) 0x40012404))				// ADC_1 Controll Register
#define ADC1_CR2  	  (*((volatile unsigned long*) 0x40012408))				// ADC_1 Controll Register 2
#define ADC1_SMPTR2   (*((volatile unsigned long*) 0x40012410))				// ADC_1 Sample Time register 2
#define ADC1_SQR1     (*((volatile unsigned long*) 0x4001242C))				// ADC_1 Regular Sequence Register 1
#define ADC1_SQR3     (*((volatile unsigned long*) 0x40012434))				// ADC_1 Regular Sequence Register 3
#define ADC1_DR       (*((volatile unsigned long*) 0x4001244C))				// ADC_1 Data Register

#define NVIC_SETENA_0 (*((volatile unsigned long*) 0xE000E100))			  	// NVIC SET-Enable Register
#define NVIC_CLRENA_0 (*((volatile unsigned long*) 0xE000E180))				// NVIC CLEAR-Enable Register


//........................................................................................................................

#define APB2_FREQ 72000000
#define APB1_FREQ 36000000

#endif