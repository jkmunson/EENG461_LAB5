//
// Created by Mark Clark on 10/19/23.
//

#include "TTYCom.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "inc/hw_uart.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_memmap.h"
#include "common/tm4c123gh6pm.h"

#define INFINITY 1.0f/0.f

void UARTConfigure(uint16_t baudrate) {
    //Enable clock for UART 0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;
    while(!(SYSCTL_PRUART_R & SYSCTL_PRUART_R0)) {}

    //Enable clock for GPIO Port A
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
    while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0)) {}

//    GPIO_PORTA_LOCK_R = 0x4C4F434B;
//    GPIO_PORTA_CR_R |= (1 << 0) | (1 << 1);
//    GPIO_PORTA_AFSEL_R &= ~((1 << 0) | (1 << 1)); //Alternate function for PA0 and PA1 (Clear bits per table on pager 671)
//    GPIO_PORTA_DEN_R &= ~((1 << 0) | (1 << 1));
//    GPIO_PORTA_PDR_R &= ~((1 << 0) | (1 << 1));
//    GPIO_PORTA_PUR_R &= ~((1 << 0) | (1 << 1));
//    GPIO_PORTA_DR8R_R |= (1 << 0) | (1 << 1);
//    GPIO_PORTA_SLR_R |= (1 << 0) | (1 << 1);
//    GPIO_PORTA_PCTL_R |= (GPIO_PORTA_PCTL_R & ~GPIO_PCTL_PA0_M) | GPIO_PCTL_PA0_U0RX;
//    GPIO_PORTA_PCTL_R |= (GPIO_PORTA_PCTL_R & ~GPIO_PCTL_PA1_M) | GPIO_PCTL_PA1_U0TX;

    float BRD = ROM_SysCtlClockGet() / (16 * baudrate);

    UART0_CTL_R &= ~UART_CTL_UARTEN; //Disable UART
    UART0_IBRD_R |= (uint16_t)BRD; //Integer portion
    UART0_FBRD_R |= ((uint8_t)((BRD-(uint16_t)BRD) * 64 + 0.5f)) & 0x3F; // & to ensure FBRD is not bigger than 6-bits wide
    UART0_LCRH_R |= UART_LCRH_WLEN_8 | UART_LCRH_FEN | UART_LCRH_PEN | UART_LCRH_EPS; //8-bit, 1 stop bit, even parity, using FIFO
    UART0_IM_R |= UART_IM_TXIM; //Enable Transmission Interrupt
    UART0_CTL_R |= UART_CTL_UARTEN; //Enable UART

}

void putchar(char c) {
    static int chars_sent_recently = 0;
    ROM_UARTCharPut(UART0_BASE, c);
    while(ROM_UARTBusy(UART0_BASE)) {};

    //Really dumb, but avoid overwhelming ICDI
    if(++chars_sent_recently > 7) {
        chars_sent_recently = 0;
        for(int i = 0; i < 150; i++) {
            __asm("mov r1,r1\n");
        }
    }
}

void print_string(const char * const str) {
    for(int i = 0; str[i] != '\0'; i++) {
        putchar( str[i]);
    }
}

void print_unsigned_decimal(uint32_t num){

    char buf[11]; //Large enough to fit any value of num

    int places = 0;

    do {
        buf[places++] = (char)('0' + (num % 10));
        num /= 10;
    } while (num > 0);

    for(; places; places--) {
        putchar( buf[places-1]);
    }
}

void print_decimal(int32_t num){

    if (num < 0) {
        putchar( '-');
        num = 0 - num;
    }

    print_unsigned_decimal((uint32_t)num);
}

void print_float(float number) {

    if(number != number) { //NaN is not equal to anything, including NaN
        print_string("NaN");
        return;
    }

    if(number == INFINITY || number == -INFINITY) {
        print_string("inf");
        return;
    }

    if(number > UINT32_MAX || (0-number) > UINT32_MAX) {
        print_string("[out of range]");
        return;
    }

    char buf[15]; // large enough to fit any value

    if(number < 0) {
        putchar( '-');
        number = 0 - number;
    }

    uint32_t integerPart = (uint32_t)number;
    uint32_t decimalPart = (uint32_t)((number - integerPart) * 1000);

    int places = 0;

    for(int i = 0; i < 3; i++) {
        buf[places++] = (char)('0' + decimalPart % 10);
        decimalPart /= 10;
    };

    buf[places++] = '.';

    print_unsigned_decimal(integerPart);

    for(; places; places--) {
        putchar( buf[places-1]);
    }
}

void printlf(char format[], ...) {
    va_list args;
    va_start(args, format);

    char *str;
    int32_t num;
    uint32_t numu;
    float *numf;

    for(int i=0; format[i] != '\0'; i++) {
        switch(format[i]) {
            case '%' :
                i++;
                switch(format[i]) {
                    case 'u': //unsigned decimal number
                        numu = va_arg(args, uint32_t);
                        print_unsigned_decimal(numu);
                        break;

                    case 'd': //signed decimal number
                        num = va_arg(args, int32_t);
                        print_decimal(num);
                        break;

                    case '\0': // End of string
                        putchar( '%');
                        i--; //let the for loop catch this
                        break;

                    case 's': //string
                        str = va_arg(args, char*);
                        print_string(str);
                        break;

                    case 'f': //float
                        numf = va_arg(args, float *);
                        print_float(*numf);
                        break;

                    default: //Not recognized
                        putchar( '%');
                        putchar( format[i]);
                        break;
                }
                break;

            default:
                putchar( format[i]);
                break;
        }
    }

    va_end(args);
}
