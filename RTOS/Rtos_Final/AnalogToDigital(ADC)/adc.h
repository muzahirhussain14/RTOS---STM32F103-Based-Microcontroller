#ifndef ADC_H_
#define ADC_H_

#include "preprocessors.h"

extern volatile int conversionResult;

void ADC1_2_IRQHandler(void);
int ADC_Init(short mode, short alignment);
void ADC_ChannelConfig(short channelNumber, short sampleTime);
void ADC_StartConversion(); 	
void ADC_Finish(short channel);

#endif