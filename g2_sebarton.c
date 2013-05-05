/* 
	Dog on the Counter Again
	16-250 Gadget 2
	Spencer Barton
 */

#include "g2.h"

int main(void)
{	
    unsigned int c, ret;
    unsigned char accel_data = 'z';

	initSystem();

	uart_puts_P("HALT is online\n");

	// set device address and read mode
	ret = i2c_start(Dev24C02+I2C_WRITE); 
	uart_puts_P("test\n");

	if ( ret ) {

		uart_puts_P("Accelerometer unavailable\n");

		// error, so stop
		i2c_stop();

	} else {

        i2c_write(0x00);                         // write address = 0
        i2c_rep_start(Dev24C02+I2C_READ);        // set device address and read mode
        accel_data = i2c_readAck();                       // read one byte form address 0
        accel_data = i2c_readAck();                       //  "    "    "    "     "    1
        accel_data = i2c_readAck();                       //  "    "    "    "     "    2
        accel_data = i2c_readNak();                       //  "    "    "    "     "    3
        i2c_stop(); 

	}
	
	while(1) {	

		if ( safeUARTgetc( &c ) == OK ) {

			if ( c == 'A' ) {

				uart_putc( '>' );
				uart_putc( (unsigned char)accel_data );
				uart_putc( '\n' );

			} else {

				// echo char
				uart_putc( (unsigned char)c );

			}

		}

	}
		
}