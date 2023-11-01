#include "main.h"
#include "gpioCode.h"
#include "timerCode.h"
#include "stepperMotorCode.h"
#include "uart_print.h"
#include <stdbool.h>
#include <common/tm4c123gh6pm.h>

int main (void) {

	//printf UART setup
    setup_uart_printer();
	
    //GPIO/Switch Configuration
	GPIOConfigure();

    //Timer Configuration
	initTimers();

    initSequenceList();
    initStepperMotor(64, 5.625f, 4, 400, 0xF0);

	Enable_Interrupts(); //Enable Global Interrupts

    calculateRotation(360.0f);

	while (1) {
        //calculateRotation(360.0f);
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
