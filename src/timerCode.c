#include <stdint.h>
#include <stdbool.h>
#include "timerCode.h"
#include "main.h"
#include "gpioCode.h"
#include "common/tm4c123gh6pm.h"

volatile uint32_t uptime_milliseconds;
volatile uint32_t uptime_300ms;

void initTimers(void) {

    /*
     * 1 second ADC trigger
     */
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;                      //Enable Run Mode Clock Gating Control for Timer 0
    while (!(SYSCTL_PRTIMER_R & SYSCTL_RCGCTIMER_R0)) {}            //Wait for peripheral to be ready
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                                //Disable Timer
    TIMER0_CTL_R |= TIMER_CTL_TAOTE;                                //Set as ADC trigger
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;                          //32-bit Timer
    TIMER0_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;                        //Set Timer to count down periodically
    TIMER0_TAILR_R = SYSCLKFREQ;                                    //Set period for 1 second
    TIMER0_CTL_R |= TIMER_CTL_TAEN;                                 //Enable Timer

    /*
     * Trigger for Stepper Motor
     */
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;                      //Enable Run Mode Clock Gating Control for Timer 0
    while (!(SYSCTL_PRTIMER_R & SYSCTL_RCGCTIMER_R1)) {}            //Wait for peripheral to be ready
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;                                //Disable Timer
    TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;                          //32-bit Timer
    TIMER1_TAMR_R |= TIMER_TAMR_TAMR_PERIOD;                        //Set Timer to count down periodically
    TIMER1_TAILR_R = SYSCLKFREQ / 800;                              //Set period for 100 Hz
    TIMER1_ICR_R |= TIMER_ICR_TATOCINT;
    TIMER1_IMR_R |= TIMER_IMR_TATOIM;
    NVIC_EN0_R |= 1 << (INT_TIMER1A - 16);
    TIMER1_CTL_R |= TIMER_CTL_TAEN;                                 //Enable Timer

}

void timeKeeperISR (void) {

    TIMER0_IMR_R &= ~TIMER_IMR_TATOIM;                                      //Disable Interrupt
    TIMER0_ICR_R |= TIMER_ICR_TATOCINT;                                     //Clear Interrupt

    uptime_milliseconds++;

    if (uptime_milliseconds % 300 == 0) {
        uptime_300ms++;
    }

    TIMER0_IMR_R |= TIMER_IMR_TATOIM;                                       //Enable Interrupt
}
