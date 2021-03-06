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
#define ERROR_DELAY 100

// Definitions for USB serial
#define UART_BAUD_RATE 9600
#define CPU_CLK F_CPU

// I2C for accelerometer
#define ACCEL_ADDR 	(0x4c << 1)		// from datasheet
#define I2C_DELAY 	100

// regs for accelerometer
#define XOUT        0x00
#define YOUT        0x01
#define ZOUT        0x02
#define TILT        0x03
#define SRST        0x04
#define SPCNT       0x05
#define INTSU       0x06
#define MODE        0x07
#define SR          0x08
#define PDET        0x09
#define PD          0x0A

// states for accel
#define	ACTIVE_MODE 	0x01
#define	STANDBY_MODE 	0x00

#define ON 1
#define OFF 0

// UART statuses for wrapper functions
#define NOTHING	0
#define ERROR  -1
#define OK		1

struct accel_data_t {
	unsigned char X;
	unsigned char Y;
	unsigned char Z;
};


void initSystem(void);
void initLED(void);
void initUART(void);
void initI2C(void);

void setLED(uint8_t);

int safeUARTgetc( int* c );

int i2cSafeStart( unsigned char addr );
int i2cWrite( unsigned char reg, unsigned char data);
unsigned char i2cRead( unsigned char reg);
int i2cReadXYZ( struct accel_data_t *accel_data );
void printXYZ( struct accel_data_t accel_data );

#endif