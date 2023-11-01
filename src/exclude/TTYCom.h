//
// Created by Mark Clark on 10/19/23.
//

#ifndef EENG461_LAB_4_TTYCOM_H
#define EENG461_LAB_4_TTYCOM_H

#include <stdint.h>

void UARTConfigure(uint16_t baudrate);

/*!
 * 	Supports %s (string), %d (signed decimal), %u (unsigned decimal), and %f (float)
 * FLOATS SHOULD BE PASSED AS A POINTER TO FLOAT - THIS IS DUE TO A C LANGUAGE LIMITATION
 * COMBINED WITH NO HARDWARE (double)
 * Floats are limited in magnitude to UINT32_MAX
 * Floats only print a fixed 3 decimal places
 * */
void printlf(char format[], ...);

void print_string(const char * const str);

#endif //EENG461_LAB_4_TTYCOM_H
