//
// Created by Mark Clark on 10/19/23.
//

#include "sonicSensorCode.h"
#include "timerCode.h"
#include "gpioCode.h"
#include "common/tm4c123gh6pm.h"

volatile uint16_t echo_time_count;
volatile uint16_t startTime;
volatile uint16_t endTime;
static enum {FALLING, RISING} prev_echo_edge;

void ultrasonicConfigure(void) {

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;                        //Enable Run Mode Clock Gate Control for GPIO Port B
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)) {}                 //Wait for peripheral to be ready

    GPIO_PORTB_DIR_R |= (1 << 0);                                   //Set PB0 as input
    GPIO_PORTB_DEN_R |= (1 << 0);                                   //Set PB0 as digital
    GPIO_PORTB_DR8R_R |= (1 << 0);                                  //Set PB0 to drive 8-mA
    GPIO_PORTB_DATA_R &= ~(1 << 0);                                 //Ensure PB0 data is cleared

    GPIO_PORTB_AFSEL_R |= (1 << 1);                                 //Set PB1 to alternate function
    GPIO_PORTB_DEN_R |= (1 << 1);                                   //Set PB1 to digital
    GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB1_T2CCP1;                      //Set PB1 to route to Timer 2

    ultraSonicEchoInCapTimerToggle(true);                    //Enable Input Capture Timer

}

void triggerSonicPulse(void) {
    GPIO_PORTB_DATA_BITS_R[(1 << 0)] ^= (1 << 0);
    ultraSonicTrigTimerToggle(true);                            //One Shot Timer for 10uS
}

void sonicTriggerISR() {
    TIMER1_IMR_R &= ~TIMER_IMR_TATOIM;                                  //Disable Interrupt
    TIMER1_ICR_R |= TIMER_ICR_TATOCINT;                                 //Clear Interrupt

    GPIO_PORTB_DATA_BITS_R[(1 << 0)] ^= (1 << 0);

    ultraSonicTrigTimerToggle(false);                            //Disable Timer

}

void sonicEchoISR() {
    TIMER2_IMR_R &= ~TIMER_IMR_CBEIM;                                   //Disable Interrupt
    TIMER2_ICR_R |= TIMER_ICR_CBECINT;                                  //Clear Interrupt

    /*
     * Check whether this trigger is the rising edgge or falling edge.
     * If rising edge, record the time as start time
     * If falling edge, record the time as end time and calculate total time betwewen rising and falling edge
     * The previous echo edge is set to the current edge type so the next trigger checks the other edge type.
     */
    switch (prev_echo_edge) {
        case RISING:
            // Record the time of the falling edge
            endTime = (TIMER2_TBR_R & 0xFFFF);
            prev_echo_edge = FALLING; //If previous edge was rising, the current edge is falling.
            // Calculate the elapsed time
            if (endTime > startTime) {
                echo_time_count = endTime - startTime;
            } else {
                // Timer overflow occurred
                echo_time_count = (0xFFFF - startTime) + endTime + 1;
            }
            break;
        case FALLING:
            // Record the time of the rising edge
            startTime = (TIMER2_TBR_R & 0xFFFF);
            prev_echo_edge = RISING; //If previous edge was falling, the current edge is rising
            break;
        default:
            startTime = (TIMER2_TBR_R & 0xFFFF);
            prev_echo_edge = RISING;
            break;
    }

    TIMER2_IMR_R |= TIMER_IMR_CBEIM;                                        //Renable Interrupt

}
