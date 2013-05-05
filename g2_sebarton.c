/* 
	Dog on the Counter Again
	16-250 Gadget 2
	Spencer Barton
 */

#include "g2.h"

int main(void)
{	
    unsigned int c, ret;
    char accel_data[7] = "empty";

	initSystem();

	uart_puts_P("HALT is online\n");

	while(1) {	

		if ( safeUARTgetc( &c ) == OK ) {

			if ( c == 'L' ) {

				setLED(ON);

			} else if ( c == 'O' ) {

				setLED(OFF);

			} else if ( c == 'X' ) {
				
				if ( (ret = i2cRead( XOUT )) != ERROR ) {
					itoa( ret, accel_data, 10);   // convert interger into string (decimal format)         
					uart_puts( accel_data );
					uart_putc( '\n' );
				}

			} else {

				// echo char
				uart_putc( (unsigned char)c );

			}
		}


	} /* end eval loop */
		
}