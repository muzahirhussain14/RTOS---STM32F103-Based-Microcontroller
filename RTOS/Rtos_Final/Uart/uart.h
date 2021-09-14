#ifndef UART_H_
#define UART_H_

#include "preprocessors.h"

//........................................UART_1................................
extern short _parityError_1;

void UART1_Send(char* data);
char UART1_Receive();
void UART1_Finish();

//You don't need to configure the GPIOS, UART1_Init function does that for you.
//port A pin 9(TX)
//port A pin 10(RX)
int UART1_Init(int baudrate, short parity, short stop);



//.......................................UART_2..................................

extern short _parityError_2;

void UART2_Send(char* data);				// returns null if there is no data or parity error occurs
char UART2_Receive();			// reads and returns the character if available else it returns null
void UART2_Finish();								// Call this function at the end to disable UART2

//You don't need to configure the GPIOS, UART2_Init function does that for you.
//UART2 TX-> PA2
//UART2 RX-> PA3

// parity -> 0 no parity, 1 even, 2 odd
// stop -> 1 (1 stop bit), 2(2 stop bit)
int UART2_Init(int baudrate, short parity, short stop);    // returns 0 for unsuccessfull else returns 1


//........................................UART_3.....................................

extern short _parityError_3;
void UART3_Send(char* data);
char UART3_Receive();

//You don't need to configure the GPIOS, UART3_Init function does that for you.

//port B pin 10(TX)
//port B pin 11(RX)
int UART3_Init(int baudrate, short parity, short stop);

#endif