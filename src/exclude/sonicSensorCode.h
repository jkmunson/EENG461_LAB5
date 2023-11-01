//
// Created by Mark Clark on 10/19/23.
//

#ifndef EENG461_LAB_4_SONICSENSORCODE_H
#define EENG461_LAB_4_SONICSENSORCODE_H

#include <stdint.h>

#include "common/tm4c123gh6pm.h"

extern volatile uint16_t echo_time_count;
extern volatile uint16_t startTime;
extern volatile uint16_t endTime;

void ultrasonicConfigure(void);
void triggerSonicPulse(void);
void sonicTriggerISR();
void sonicEchoISR();

#endif //EENG461_LAB_4_SONICSENSORCODE_H
