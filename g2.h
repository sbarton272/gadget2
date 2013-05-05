#ifndef __GADGET2_H__
#define __GADGET2_H__

// uncomment if you're getting an error related to delay
//#define __DELAY_BACKWARD_COMPATIBLE__

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.h"
#include "i2cmaster.h"

// Some convenient definitions
#define LED PC0

// Definitions for USB serial
#define UART_BAUD_RATE 9600
#define CPU_CLK F_CPU

// I2C
#define Dev24C02  0xA2      // device address of EEPROM 24C02, see datasheet

#define ON 1
#define OFF 0

// UART statuses for wrapper functions
#define NOTHING	0
#define ERROR  -1
#define OK		1

void initSystem(void);
void initLED(void);
void initUART(void);

void setLED(uint8_t);

int safeUARTgetc( int* c );

#endif