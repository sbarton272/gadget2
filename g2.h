#ifndef __GADGET2_H__
#define __GADGET2_H__

// uncomment if you're getting an error related to delay
//#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <util/delay.h>

// Some convenient definitions
#define LED PC0

#define ON 1
#define OFF 0

void initSystem(void);
void initLED(void);

void setLED(uint8_t);

#endif