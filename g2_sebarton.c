/* 
	Dog on the Counter Again
	16-250 Gadget 2
	Spencer Barton
 */

#include "g2.h"

int main(void)
{	

	initSystem();

	while(1) {	

		// light LED
		setLED(ON);
		_delay_ms(100);
		setLED(OFF);
		_delay_ms(100);

	}
		
}