#include "g2.h"

/* ==========================
 * ===== Init Functions =====
 * ========================== */

/* Run all of the init functions */
void initSystem() {

	initLED();

}


/* In this function you should set the LED pins to be outputs and
   set them high to turn them off (because our LEDs are active low)
 */
void initLED()
{
	// Set LED pin direction to output
	DDRC |= _BV(LED);
	
	// Start LEDs ON to signal device is on
	setLED(ON);
	
}


/* ==========================
 * ===== LED Controls =======
 * ========================== */

/*  set the led value */
void setLED(uint8_t led)
{
	if(led) {
		PORTC |= _BV(LED);
	}
	else {
		PORTC &= ~_BV(LED);
	}
}
