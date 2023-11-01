//
// Created by Mark Clark on 10/19/23.
//

#ifndef EENG461_LAB_4_ADCCODE_H
#define EENG461_LAB_4_ADCCODE_H

#include <stdint.h>

extern volatile uint16_t potReading;

void ADCPinConfigure(void);
void ADCSampleSequencerConfigure(void);
void saveADCSample(void);

#endif //EENG461_LAB_4_ADCCODE_H
