#ifndef EENG461_LAB_5_STEPPERMOTORCODE_H
#define EENG461_LAB_5_STEPPERMOTORCODE_H

struct stepperMotor;
struct stepperMotor* initStepperMotor(uint8_t num_strides, float stride_angle, uint8_t num_phases, uint8_t pulse_freq, uint32_t gpio_val);
void calculateRotation(struct stepperMotor* stepper_motor, float angle);
void rotate(struct stepperMotor* stepper_motor);

#endif //EENG461_LAB_5_STEPPERMOTORCODE_H
