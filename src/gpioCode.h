#pragma once

#ifndef EENG461_LAB_4_GPIOCODE_H
#define EENG461_LAB_4_GPIOCODE_H

#define GREEN_LED (1 << 3)
#define BLUE_LED (1 << 2)
#define RED_LED (1 << 1)
#define RGB_PINS GREEN_LED | BLUE_LED | RED_LED
#define SW1_PIN (1 << 4)

#define GPIO_PIN5 (1 << 5)

void GPIOConfigure(void);
void toggleRedLED(void);
void toggleBlueLED(void);
void toggleGreenLED(void);

#endif //EENG461_LAB_4_GPIOCODE_H
