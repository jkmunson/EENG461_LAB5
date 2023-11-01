//
// Created by Mark Clark on 10/19/23.
//

#include "adcCode.h"
#include <stdint.h>
#include "adcCode.h"
#include "timerCode.h"
#include "common/tm4c123gh6pm.h"

#define GPIO_PIN4 (1 << 4)
#define POT_TRIGGER_MARGIN 0xA

volatile uint16_t potReading;

void ADCPinConfigure(void) {

    SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R1;                      //Enable ADC Clock
    while(!(SYSCTL_PRADC_R & SYSCTL_PRADC_R1)) {};              //Wait for peripheral to be ready

    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;                    //Enable GPIO Pin for ADC (PE5)
    while(!(SYSCTL_RCGCGPIO_R & SYSCTL_RCGCGPIO_R4)) {};        //Wait fo peripheral to be ready

    GPIO_PORTE_AFSEL_R |= GPIO_PIN4;                            //Set Alternate Function Select
    GPIO_PORTE_DEN_R &= ~GPIO_PIN4;                             //Clear Digital Enable for Pin 5
    GPIO_PORTE_AMSEL_R |= GPIO_PIN4;                            //Set Alternate Mode Select

}

void ADCSampleSequencerConfigure(void) {

    ADC1_ACTSS_R &= ~ADC_ACTSS_ASEN3;                           //Disable Sequencer 3
    ADC1_EMUX_R |= ADC_EMUX_EM3_TIMER;                          //Set ADC as Timer Triggered
    ADC1_SSMUX3_R |= 0x9;                                       //Enable AIN9
    ADC1_SSCTL3_R |= ADC_SSCTL3_IE0 | ADC_SSCTL3_END0;          //Sequencer control
    ADC1_SAC_R = 0x6;                                           //Enables x64 Oversampling
    ADC1_ISC_R |= ADC_ISC_IN3;                                  //Clear Interrupt
    ADC1_IM_R |= ADC_IM_MASK3;                                  //Enable Interrupt
    NVIC_EN1_R |= 1 << (INT_ADC1SS3 - 16 - 32);                 //Enable NVIC for ADC1 Sequencer 3

    //configureAdcTimer();

    ADC1_ACTSS_R |= ADC_ACTSS_ASEN3;                            //Enable Sequencer

}

void saveADCSample(void){

    ADC1_IM_R &= ~ADC_IM_MASK3;                                 //Disable Interrupt
    ADC1_ISC_R |= ADC_ISC_IN3;                                  //Clear Interrupt

    potReading = (ADC1_SSFIFO3_R & ADC_SSFIFO3_DATA_M);         //Read Potentiometer Value

    ADC1_IM_R |= ADC_IM_MASK3;                                  //Enable Interrupt

}
