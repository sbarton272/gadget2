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


        i2c_start_wait(Dev24C02+I2C_WRITE);      // set device address and write mode
        i2c_write(0x00);                         // write address = 0
        i2c_rep_start(Dev24C02+I2C_READ);        // set device address and read mode
        accel_data = i2c_readAck();                       // read one byte form address 0
        accel_data = i2c_readAck();                       //  "    "    "    "     "    1
        accel_data = i2c_readAck();                       //  "    "    "    "     "    2
        accel_data = i2c_readNak();                       //  "    "    "    "     "    3
        i2c_stop();   

		if ( safeUARTgetc( &c ) == OK ) {

			if ( c == 'A' ) {

				uart_puts("Found A\n");

			}

			// echo char
			uart_putc( (unsigned char)c );
			uart_putc( ':' );
			uart_putc( (unsigned char)accel_data );
			uart_putc( ';' );

		}

	}
		
}