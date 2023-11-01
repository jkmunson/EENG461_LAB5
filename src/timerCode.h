//
// Created by Mark Clark on 10/19/23.
//

#ifndef EENG461_LAB_4_TIMERCODE_H
#define EENG461_LAB_4_TIMERCODE_H

#include <stdint.h>
#include <stdbool.h>

extern volatile uint32_t uptime_milliseconds;
extern volatile uint32_t uptime_300ms;

void initTimers(void);
void ultraSonicTrigTimerToggle(bool enable);
void ultraSonicEchoInCapTimerToggle(bool enable);
void timeKeeperISR (void);

#endif //EENG461_LAB_4_TIMERCODE_H
