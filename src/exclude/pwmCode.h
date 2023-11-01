#include <stdint.h>
#ifndef EENG461_LAB_3_PWMCODE_H
#define EENG461_LAB_3_PWMCODE_H

#define RED_LED (1 << 1)
#define CYCLES_PER_MS 1000
#define CYCLES_PER_US 1

//Servo Globals
#define SERVO_PERIOD 20000     // 20ms period
#define MIN_PULSE 500         // -90deg pulse (~0.5ms pulse)
#define MAX_PULSE 2500         // 90deg pulse (~2.5ms pulse)
#define DEG_OF_ROTATION 180 // How many degrees the servo can rotate

void PWMConfigure(void);
void PWMSetPeriod(uint16_t cycles_per_period);
void PWMSetDutyCycle(uint16_t duty_cycle);
void PWMEnable(void);
void PWMDisable(void);

#endif //EENG461_LAB_3_PWMCODE_H
