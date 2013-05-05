/* 
	Dog on the Counter Again
	16-250 Gadget 2
	Spencer Barton
 */

#include "g2.h"

int main(void)
{	
    unsigned int c;
    unsigned char accel_data;

	initSystem();

	// set device address and read mode
	if ( !i2c_start(Dev24C02+I2C_READ) ) {
		// error, so stop
		i2c_stop();
	}

	uart_puts("HALT is online\n");

	while(1) {	

		if ( safeUARTgetc( &c ) == OK ) {

			if ( c == 'A' ) {

				uart_puts("Found A\n");

			}

			// echo char
			uart_putc( (unsigned char)c );

		}

	}
		
}