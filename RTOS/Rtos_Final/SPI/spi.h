#ifndef SPI_H_
#define SPI_H_

#include "preprocessors.h"

/*
Standard Settings:

1) Full duplex
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
		Connect SS_2 to PB4
		(SCK, MISO, MOSI are common)

	->Slave 3:
		Connect SS_3 to PC4
		(SCK, MISO, MOSI are common)
	  Connect the slaves in order, e.g. if you want to connect only one slave, use Slave 1 settings, if you want to 
	  connect 2 slaves use slave 1 and slave 2 settings and so on.
	
7) In Slave Mode, use follwoing GPIO pins,
	-> Connect SS_1(Slave Select) to PA4
	-> Connect SCK(source clock) to PA5
	-> Connect MISO(Master In Slave Out) to PA6
	-> Connect MOSI(Master Out Slave In) to PA7	
*/

void Init_Master(short baudrate, short LSB_First, short mode, int numbOfSlaves);
void Init_Slave(short nss, short LSB_First, short mode, short multiSlave);		// Slave Mode
unsigned char SPI_Transfer(unsigned char data);		//Send/Receive(Data is send and received Byte to Byte)
void Enable_Slave(short slaveNumb);
void Disable_Slave(short slaveNumb);
void SPI_Finish();

#endif