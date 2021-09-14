/*
Fixed Settings:

1) 2 Way, Full duplex
2) 8 bit data can be transmit/receive
3) SPI 1 is used
4) In master mode, NSS will be NSS_soft, but in slave mode you can select either hardware nss or software nss
5) No Multi-Master mode

6) You can attach upto 3 slaves in master mode
	->Slave 1:
		Connect SS_1(Slave Select for 1st Slave) to PA4
		Connect SCK(source clock) to PA5
		Connect MISO(Master In Slave Out) to PA6
		Connect MOSI(Master Out Slave In) to PA7

	->Slave 2:
		Connect SS_2 to PB6
		(SCK, MISO, MOSI are common)

	->Slave 3:
		Connect SS_3 to PC7
		(SCK, MISO, MOSI are common)
	  Connect the slaves in order, e.g. if you want to connect only one slave, use Slave 1 settings, if you want to 
	  connect 2 slaves use slave 1 and slave 2 settings and so on.
	
7) In Slave Mode, use follwoing GPIO pins,
	-> Connect SS_1(Slave Select) to PA4
	-> Connect SCK(source clock) to PA5
	-> Connect MISO(Master In Slave Out) to PA6
	-> Connect MOSI(Master Out Slave In) to PA7	
*/


#include "spi.h"

/*
Function Discription:
	Call this function if you want your micro-controller as master. You don't need to configure GPIOs. This funtion does that for you. 
	Also note that SPI1 runs at 48Mhz PLL clock source(provided by HCI) instead of usual 8Mhz HCI. Using SPI1 might effect other peripherals that are configured to run with default 8Mhz HSI clock source.

Parameters:
	-> baudrate = 0,1,2,3,4,5,6,7
		 0: fPCLK/2
		 1: fPCLK/4
		 2: fPCLK/8
		 3: fPCLK/16
		 4: fPCLK/32
		 5: fPCLK/64
		 6: fPCLK/128
		 7: fPCLK/256
         fPCLK = 48Mhz
		 
	-> LSB_First = 0,1
		 0: MSB
		 1: LSB
	
  -> mode = 0,1,2,3
		 0: CPOL = 0, CPHA = 0
		 1: CPOL = 0, CPHA = 1
		 2: CPOL = 1, CPHA = 0
		 3: CPOL = 1, CPHA = 1
		 CPOL = Clock Polarity, CPHA = Clock Phase
		 
	-> numbOfSlaves: 1,2,3 (Maximum 3 slaves can be attached)
		 Enter the number of slaves connected to the master.
		 1: GPIO of first slave will be initlized
		 2: GPIO of first and second slaves will be initlized
		 3: GPIO of first and second and third slaves will be initlized
*/

void Init_Master(short baudrate, short LSB_First, short mode, int numbOfSlaves)
{
    /* Changing Clock Source To 48MHz PLL*/
	
	// PLL multiplies by factor of 12          
    RCC_CFGR   |= 0x00280000;
		
	// HSI frequency divided is divided by 2 by default before entering PLL
    // activate pll
	RCC_CR |= 0x01000000;
	while((RCC_CR & 0x02000000) == 0); // wait until PLL is not locked
	RCC_CFGR |= 0x00000002;   // use PLL as system clock
	
	// enable clock for SPI_1
	RCC_APB2ENR |= 0x00001000;
	SPI_CR1 &= 0x00000000;					// clearing the SPI Control Register, DFF bit is set to 8 bit by default
	
	//setting the baudrate
	SPI_CR1 = baudrate;
	SPI_CR1 = SPI_CR1<<3;						// shiefting bits (3 places to left)

	// Setting the mode(Clock polarity, Clock phase)
	if(mode == 0)
		SPI_CR1 |= 0x00000000;				// CPOL = 0, CPHA = 0
	
	else if(mode == 1)
		SPI_CR1 |= 0x00000001;				// CPOL = 0, CPHA = 1
	
	else if(mode == 2)
		SPI_CR1 |= 0x00000002;				// CPOL = 1, CPHA = 0
	
	else if(mode == 3)
		SPI_CR1 |= 0x00000003;				// CPOL = 1, CPHA = 1
	
	//Setting LSB/MSB
	if(LSB_First == 1)						// LSB
		SPI_CR1 |= 0x00000080;
	
	
	// Configuring GPIO
	RCC_APB2ENR |= 0x00000004;					// port A clock enable
	GPIOA_CRL &= 0x0000FFFF;
	
	// SPI_SCK
	GPIOA_CRL |= 0x00B00000;				//Alternate function push-pull, 50mhz
	
	// SPI_MOSI
	GPIOA_CRL |= 0xB0000000;				//Alternate function push-pull, 50mhz
	
	// SPI_MISO
	GPIOA_CRL |= 0x04000000;				//Input floating/Input pull-up

	// Slave 1
	GPIOA_CRL |= 0x00030000;				//SPI_Nss -> General purpose output push-pull(software mode), 50mhz
		
	//Disabling the slaves(set pin to HIGH)
	GPIOA_BSRR = 0x00000010;
		
	//Slave 2
	if(numbOfSlaves > 1)
	{
		RCC_APB2ENR |= 0x00000008;					// port B clock enable
		GPIOB_CRL &= 0xF0FFFFFF;
		GPIOB_CRL |= 0x03000000;					// Configuring PB6 for SS_2 
		
		//Disabling the slaves(set pin to HIGH)
	    GPIOB_BSRR = 0x00000040;
	}
	
	//Slave 3
	if(numbOfSlaves > 2)
	{
		RCC_APB2ENR |= 0x00000010;					// port C clock enable
		GPIOC_CRL &= 0x0FFFFFFF;
		GPIOC_CRL |= 0x30000000;					// Configuring PC7 for SS_3
		
		//Disabling the slaves(set pin to HIGH)
	  GPIOC_BSRR = 0x00000080;
	}
	
	SPI_CR1 |= 0x00000344;					// Enable master and SPE, Software Slave management=1, Internal Slave Select=1	
}


/*
Discription:
	Call this function if you want your micro-controller as slave.

Parameters:
	-> nss = 0,1
     0: Hardware Nss
     1: Software Nss

	-> LSB_First = 0,1
		 0: MSB
		 1: LSB
	
    -> mode = 0,1,2,3
		 0: CPOL = 0, CPHA = 0
		 1: CPOL = 0, CPHA = 1
		 2: CPOL = 1, CPHA = 0
		 3: CPOL = 1, CPHA = 1
		 CPOL = Clock Polarity, CPHA = Clock Phase
	
	-> multiSlave = 0,1
		0 = only 1 Slave(this device) is connected to master (Point to point)
		1 = Multiple Slaves are connected to master
*/

void Init_Slave(short nss, short LSB_First, short mode, short multiSlave)		// Slave Mode
{
	// enable clock for SPI_1
	RCC_APB2ENR |= 0x00001000;
	SPI_CR1 &= 0x00000000;					// clearing the SPI Control Register, DFF bit is set to 8 bit by default

	// Setting the mode(Clock polarity, Clock phase)
	if(mode == 0)
		SPI_CR1 |= 0x00000000;				// CPOL = 0, CPHA = 0
	
	else if(mode == 1)
		SPI_CR1 |= 0x00000001;				// CPOL = 0, CPHA = 1
	
	else if(mode == 2)
		SPI_CR1 |= 0x00000002;				// CPOL = 1, CPHA = 0
	
	else if(mode == 3)
		SPI_CR1 |= 0x00000003;				// CPOL = 1, CPHA = 1
	
	//Setting LSB/MSB
	if(LSB_First == 1)						// LSB
		SPI_CR1 |= 0x00000080;

	// Configuring GPIO
	RCC_APB2ENR |= 0x00000004;					// port A clock enable
	GPIOA_CRL &= 0x0000FFFF;
	
	// SPI_SCK
	GPIOA_CRL |= 0x00400000;				//Input floating

	// SPI_MOSI
	GPIOA_CRL |= 0x40000000;				//Input floating
		

	if(multiSlave == 0)						// point to point
		// SPI_MISO
		GPIOA_CRL |= 0x0B000000;			// Alternate function push-pull, 50mhz
	
	else if(multiSlave == 1)				// Multiple Slaves
		// SPI_MISO
		GPIOA_CRL |= 0x0F000000;			// Alternate function open-drain
	

	if(nss == 0)							// Hardware Nss
	{
		GPIOA_CRL |= 0x00040000;			// SPI_Nss -> Input floating (hardware mode)
		SPI_CR1 |= 0x00000040;				// Enable slave and SPE, Software Slave management=0
	}
	
	else if(nss == 1)						// Software Nss
	{
		GPIOA_CRL |= 0x00080000;			// SPI_Nss -> Input with pull-up / pull-down(software mode)		
		SPI_CR1 |= 0x00000240;				// Enable slave and SPE, Software Slave management=1		
	}
}

/*
Discription:
	Only 8 bit data will be sent/receive. The function sends/receives data simultaneously.
*/
unsigned char SPI_Transfer(unsigned char data)		//Send/Receive(Data is send and received Byte to Byte)
{
	SPI_DR = data;
	
	while((SPI_SR & 0x00000002) == 0);					// wait until transmition is complete	
	while((SPI_SR & 0x00000001) == 0);					// wait while Receive buffer is empty
	return SPI_DR;								 	    // return the received data
}

/*
Discription:
	Use this function in master mode to enable a slave before transmission/reception

Parameter:
	->slaveNumb: 1,2,3
		Enter the number of slave you want to enable
		1: First Slave will be enabled
		2: Second Slave will be enabled
		3: Third Slave will be enabled
*/
void Enable_Slave(short slaveNumb)
{
	if(slaveNumb == 1)
		GPIOA_BSRR = 0x00100000;										// Enable 1st slave
	
	else if(slaveNumb == 2)
		GPIOB_BSRR = 0x00400000;										// Enable 2nd slave
	
	else if(slaveNumb == 3)
		GPIOC_BSRR = 0x00800000;										// Enable 3rd slave
}

/*
Discription:
	Use this function in master mode to disable a slave after transmission/reception.
*/
void Disable_Slave(short slaveNumb)
{
	if(slaveNumb == 1)
		GPIOA_BSRR = 0x00000010;										// disable 1st Slave
	
	else if(slaveNumb == 2)
		GPIOB_BSRR = 0x00000040;										// disable 2nd Slave

	else if(slaveNumb == 3)
		GPIOC_BSRR = 0x00000080;										// disable 3rd Slave
}
	
/*
Discription:
	Use this function at the end to terminate the SPI. This will reset the source clock as well
*/	
void SPI_Finish()
{
	while((SPI_SR & 0x00000080) != 0);			  	// wait if Spi is busy
	SPI_CR1 &= 0x00000000;
	RCC_APB2ENR &= 0xFFFFEFFF;						// disabling the spi clock
	
	// reset to HSI 8 Mhz Clock source	
	RCC_CFGR &= 0xFFFFFFFC;   				// use HSI as system clock
	RCC_CR &= 0xFEFFFFFF;					// Pll Off
}