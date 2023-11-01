#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include "gpioCode.h"
#include "stepperMotorCode.h"
#include "uart_print.h"
#include "common/tm4c123gh6pm.h"

//#define num_strides 64                          //Number of strides
//#define stride_angle 5.625f                     //Degrees per stride
//#define num_phases 4                            //Number of phases
//#define pulse_freq 100                          //frequency of the pulses

struct stepperMotor {
    uint8_t num_strides;
    float stride_angle;
    uint8_t num_phases;
    uint32_t pulse_freq;
    struct sequenceList* sequence;
    struct sequenceStep* current_step;
    float current_angle;
    enum{PORT_A, PORT_B, PORT_C, PORT_D, PORT_E, PORT_F} gpio_port;
    uint32_t gpio_val;

    int32_t pending_steps;
    bool direction;

};

struct sequenceList {
    struct sequenceStep* head;
};

struct sequenceStep {
    struct sequenceStep* previous;
    struct sequenceStep* next;
    uint32_t value;
};

struct sequenceList sequence;
struct sequenceStep step_0, step_1, step_2, step_3, step_4, step_5, step_6, step_7;
volatile struct stepperMotor stepper_motor_1;

void initSequenceList() {
    sequence.head = &step_0;

    step_0.previous = &step_7;
    step_0.next = &step_1;
    step_0.value = 5;

    step_1.previous = &step_0;
    step_1.next = &step_2;
    step_1.value =4; //4

    step_2.previous = &step_1;
    step_2.next = &step_3;
    step_2.value = 6;

    step_3.previous = &step_2;
    step_3.next = &step_4;
    step_3.value = 2; //2

    step_4.previous = &step_3;
    step_4.next = &step_5;
    step_4.value = 10;

    step_5.previous = &step_4;
    step_5.next = &step_6;
    step_5.value = 8; //8

    step_6.previous = &step_5;
    step_6.next = &step_7;
    step_6.value = 9;

    step_7.previous = &step_6;
    step_7.next = &step_0;
    step_7.value = 1; //1
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
    stepper_motor_1.pending_steps = 2048 * 2;
    //stepper_motor_1.pending_steps = ((angle - stepper_motor_1.current_angle) / stepper_motor_1.stride_angle) * 2;
    //stepper_motor_1.current_angle = angle;
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
