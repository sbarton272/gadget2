/* 
	Dog on the Counter Again
	16-250 Gadget 2
	Spencer Barton
 */

#include "g2.h"

int main(void)
{	
    unsigned int c;
    struct accel_data_t accel_data = { 0 };
	
	initSystem();

	uart_puts_P("HALT is online\n");
	_delay_ms(1000);
	

	while(1) {	

		if ( safeUARTgetc( &c ) == OK ) {

			if ( c == 'l' ) {

				setLED(ON);

			} else if ( c == 'o' ) {

				setLED(OFF);

			} else if ( c == 'x' ) {
				
				if ( i2cReadXYZ( &accel_data ) != ERROR) {
					printXYZ( accel_data );
				}

			} else {

				// echo char
				uart_putc( (unsigned char)c );

			}
		}

	} /* end eval loop */
		
}