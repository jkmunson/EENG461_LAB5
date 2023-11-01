#ifndef EENG461_LAB_5_STEPPERMOTORCODE_H
#define EENG461_LAB_5_STEPPERMOTORCODE_H

extern volatile struct stepperMotor stepper_motor_1;

struct stepperMotor;
struct sequenceList;
struct sequenceStep;

void initSequenceList();
void initStepperMotor(uint8_t num_strides, float stride_angle, uint8_t num_phases, uint32_t pulse_freq, uint32_t gpio_val);
void calculateRotation(float angle);
void rotate();

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

#endif //EENG461_LAB_5_STEPPERMOTORCODE_H
