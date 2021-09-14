#ifndef DAC_H_
#define DAC_H_

#include "preprocessors.h"

int DAC_Init(short channel);
void DAC_Convert(unsigned int data, short alignment, short channel);
void DAC_Finish();

#endif