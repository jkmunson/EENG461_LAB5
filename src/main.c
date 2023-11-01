#include "main.h"
#include "gpioCode.h"
#include "timerCode.h"
#include "stepperMotorCode.h"
#include "uart_print.h"
#include <stdbool.h>
#include <common/tm4c123gh6pm.h>
#include <adcCode.h>

int main (void) {

	//printf UART setup
    setup_uart_printer();
	
    //GPIO/Switch Configuration
	GPIOConfigure();

    //Timer Configuration
	initTimers();
	
	ADCPinConfigure();
	ADCSampleSequencerConfigure();

    initSequenceList();
    initStepperMotor(64, 5.625f/64.0f, 4, 400, 0xF0);

	Enable_Interrupts(); //Enable Global Interrupts


	int adc_lowest = 500; //recalibrate on the fly
	int adc_highest = 3000;
	
	uint32_t last_time = uptime_seconds;
	
	while (1) {
		 while(last_time == uptime_seconds) {};
		last_time = uptime_seconds;
		
		int temp = light;
		if(temp < adc_lowest) adc_lowest = temp; //If this is a new lowest value ever seen, set a new floor
		if(temp > adc_highest) adc_highest = temp;
		
		int target_position = ((temp - adc_lowest)*100/(adc_highest-adc_lowest) +2) / 10;
		
		printlf("Target Position: %d \t | Light %d0% | raw adc: %d\n", target_position, target_position, light);
		
		calculateRotation(target_position*360.0f);
		
	}

	return (0);
}

/*
 * Taken from Lab Assignment
 */
void Disable_Interrupts(void) {
	__asm ("  CPSID	I\n");
}

void Enable_Interrupts(void) {
	__asm ("  CPSIE	I\n");
}
