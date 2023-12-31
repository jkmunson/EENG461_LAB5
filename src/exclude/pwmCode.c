#include <stdint.h>
#include "pwmCode.h"
#include "common/tm4c123gh6pm.h"

void PWMConfigure(void) {
    /*
     * Servo is connected to (PE5) is on M0PWM5G2
     */

    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;                                  //Enable PWM Module 0
    while(!(SYSCTL_PRPWM_R & SYSCTL_PRPWM_R0)) {}                           //Wait for peripheral to be ready

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;                                //Enable GPIO Port E
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4)) {}                         //Wait for peripheral to be ready

    GPIO_PORTE_AFSEL_R |= (1 << 5);                                         //Set Alternate Function for PE5
    GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE5_M0PWM5;                              //Set Port Control to PE5 PWM value
    GPIO_PORTE_DEN_R |= (1 << 5);                                           //Set Digital Enable for PE5

    SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~SYSCTL_RCC_PWMDIV_M) | SYSCTL_RCC_PWMDIV_16;

    PWM0_2_CTL_R = 0x0;                                                     //Disable PWM0 Gen2
    PWM0_2_GENB_R = PWM_2_GENB_ACTCMPBD_ONE | PWM_2_GENB_ACTLOAD_ZERO | PWM_2_GENB_ACTZERO_ONE;
                                                                            //Set PWM1 Gen2-B comparator, load, and zero

    PWM0_2_LOAD_R = CYCLES_PER_US * SERVO_PERIOD;                           //Set PWM Gen2 Period to 20ms
    PWM0_2_CMPB_R = CYCLES_PER_US * (MAX_PULSE-MIN_PULSE) / 2;              //Set initial duty cycle to ~90deg

    PWMEnable();                                                            //Enable PWM

}

void PWMSetPeriod(uint16_t cycles_per_period) {
    PWMDisable();                                                           //Disable PWM
    PWM0_2_LOAD_R = cycles_per_period;                                      //Set new period
    PWMEnable();                                                            //Enable PWM
}

void PWMSetDutyCycle(uint16_t duty_cycle) {
    PWMDisable();                                                           //Disable PWM
    PWM0_2_CMPB_R = duty_cycle * CYCLES_PER_US * SERVO_PERIOD / 100;           //Set new duty cycle
    PWMEnable();                                                            //Enable PWM
}

void PWMEnable(void) {
    PWM0_2_CTL_R |= PWM_2_CTL_ENABLE;                                       //Enable PWM0 Gen 2
    PWM0_ENABLE_R |= PWM_ENABLE_PWM5EN;                                     //Enable PWM0 Output 5
}

void PWMDisable(void) {
    PWM0_2_CTL_R &= ~PWM_2_CTL_ENABLE;                                      //Disable PWM0 Gen 2
    PWM0_ENABLE_R &= ~PWM_ENABLE_PWM5EN;                                    //Disable PWM0 Output 5
}
