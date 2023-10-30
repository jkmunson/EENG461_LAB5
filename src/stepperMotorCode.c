#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stepperMotorCode.h"
#include "common/tm4c123gh6pm.h"

//#define num_strides 64                          //Number of strides
//#define stride_angle 5.625f                     //Degrees per stride
//#define num_phases 4                            //Number of phases
//#define pulse_freq 100                          //frequency of the pulses

struct stepperMotor {
    uint8_t num_strides;
    float stride_angle;
    uint8_t num_phases;
    uint8_t pulse_freq;
    uint8_t* sequence;
    uint8_t current_step;
    enum{PORT_A, PORT_B, PORT_C, PORT_D, PORT_E, PORT_F} gpio_port;
    uint32_t gpio_val;

    uint32_t pending_move;

};

struct stepperMotor* initStepperMotor(uint8_t num_strides, float stride_angle, uint8_t num_phases, uint8_t pulse_freq, uint32_t gpio_val) {
    struct stepperMotor* tmp = malloc(sizeof(struct stepperMotor));
    tmp->num_strides = num_strides;
    tmp->stride_angle = stride_angle;
    tmp->num_phases = num_phases;
    tmp->pulse_freq = pulse_freq;
    tmp->sequence = (uint8_t *)malloc(sizeof(uint8_t) * num_phases * 2);
    uint8_t sequence[] = {5, 4, 6, 2, 10, 8, 9, 1};
    for(uint8_t i = 0; i < num_phases * 2; i++) {
        tmp->sequence[i] = sequence[i];
    }

    tmp->gpio_port = PORT_C;
    tmp->gpio_val = gpio_val;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R2)) {}

    GPIO_PORTC_DIR_R |= gpio_val;
    GPIO_PORTC_DEN_R |= gpio_val;
    GPIO_PORTC_DR8R_R |= gpio_val;
    GPIO_PORTC_DATA_BITS_R[gpio_val] = gpio_val;

    return tmp;
}

void calculateRotation(struct stepperMotor* stepper_motor, float angle) {
    stepper_motor->pending_move = (uint8_t)(angle / stepper_motor->stride_angle); //
}

void rotate(struct stepperMotor* stepper_motor) {
    uint32_t tmp_val;
    switch (stepper_motor->gpio_port) {
        case PORT_C:
            while (stepper_motor->pending_move > 0) {
                //stepper_motor->sequence[i] << 4
                stepper_motor->pending_move--;
            }
            break;
        default:
            break;
    }
}
