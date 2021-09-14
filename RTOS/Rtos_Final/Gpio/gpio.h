#ifndef GPIO_H_
#define GPIO_H_

#include "preprocessors.h"

extern volatile unsigned long* GPIO_BASE;			// points to GPIOA_CRL

void GPIO_Init(char port, short mode, short operation, short pinNumber);
void GPIO_EnablePin(char port, short pinNumber);
void GPIO_DisablePin(char port, short pinNumber);

#endif