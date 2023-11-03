#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include "gpioCode.h"
#include "stepperMotorCode.h"
#include "uart_print.h"
#include "common/tm4c123gh6pm.h"

//#define num_strides 64						  //Number of strides
//#define stride_angle 5.625f					 //Degrees per stride
//#define num_phases 4							//Number of phases
//#define pulse_freq 100						  //frequency of the pulses

struct sequenceList {
	struct sequenceStep* head;
};

struct sequenceStep {
	struct sequenceStep* previous;
	struct sequenceStep* next;
	uint32_t value;
};

struct sequenceList sequence;
static struct sequenceStep steps[8];
volatile struct stepperMotor stepper_motor_1;

void initSequenceList() {
	//sequence.head = &step_0;
	sequence.head = steps;
	
	for(int i = 0; i < 8; i++) {
		steps[i].next = &steps[(i+1)%8];
		steps[i].previous = &steps[(i+7)%8];
	}
	
	steps[0].value = 0b1001;
	steps[1].value = 0b0001;
	steps[2].value = 0b0011;
	steps[3].value = 0b0010;
	steps[4].value = 0b0110;
	steps[5].value = 0b0100;
	steps[6].value = 0b1100;
	steps[7].value = 0b1000;
}

void initStepperMotor(uint8_t num_strides, float stride_angle, uint8_t num_phases, uint32_t pulse_freq, uint32_t gpio_val) {
	stepper_motor_1.num_strides = num_strides;
	stepper_motor_1.stride_angle = stride_angle;
	stepper_motor_1.num_phases = num_phases;
	stepper_motor_1.pulse_freq = pulse_freq;
	stepper_motor_1.sequence = &sequence;
	stepper_motor_1.current_step = sequence.head;
	stepper_motor_1.pending_steps = 0;
	stepper_motor_1.direction = false;
	stepper_motor_1.current_angle = 0.0f;

	stepper_motor_1.gpio_port = PORT_C;
	stepper_motor_1.gpio_val = gpio_val;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
	while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2)) {}

	GPIO_PORTC_DIR_R |= gpio_val;
	GPIO_PORTC_DEN_R |= gpio_val;
	GPIO_PORTC_DR8R_R |= gpio_val;
	GPIO_PORTC_DATA_BITS_R[gpio_val] = 0;

}

void calculateRotation(float angle) {
	stepper_motor_1.pending_steps += ((angle - stepper_motor_1.current_angle) / stepper_motor_1.stride_angle);

	stepper_motor_1.current_angle = angle;
}

void rotate() {
	TIMER1_IMR_R &= ~TIMER_IMR_TATOIM;
	TIMER1_ICR_R |= TIMER_ICR_TATOCINT;

	if (stepper_motor_1.pending_steps > 0) {
		GPIO_PORTC_DATA_BITS_R[stepper_motor_1.gpio_val] = stepper_motor_1.current_step->value << 4;
		stepper_motor_1.current_step = stepper_motor_1.current_step->next;
		stepper_motor_1.pending_steps--;
	} else if (stepper_motor_1.pending_steps < 0) {
		GPIO_PORTC_DATA_BITS_R[stepper_motor_1.gpio_val] = stepper_motor_1.current_step->value << 4;
		stepper_motor_1.current_step = stepper_motor_1.current_step->previous;
		stepper_motor_1.pending_steps++;
	} else {
		GPIO_PORTC_DATA_BITS_R[stepper_motor_1.gpio_val] = 0x0; //Turn coils off when move is completed
	}

	TIMER1_IMR_R |= TIMER_IMR_TATOIM;
}
