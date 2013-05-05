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

	// set device in active mode
	if ( i2c_start( ACCEL_ADDR + I2C_WRITE ) ) {

		uart_puts_P("Accelerometer unavailable\n");

		// error, so stop
		i2c_stop();

	} else {

        i2c_write( MODE );
        i2c_write( ACTIVE_MODE ); // set to active mode
        i2c_stop(); 

		uart_puts("Accelerometer online\n");
	}

	
	while(1) {	

		if ( safeUARTgetc( &c ) == OK ) {

			if ( c == 'A' ) {

				uart_putc( '>' );

			} else {

				// echo char
				uart_putc( (unsigned char)c );

			}

		}

		// set device address and read mode
		if ( i2c_start( ACCEL_ADDR + I2C_READ ) ) {

			uart_puts_P("Accelerometer unavailable\n");

			// error, so stop
			i2c_stop();

		} else {


	        i2c_write( XOUT );     // set to read from X data
	        
	        i2c_rep_start( ACCEL_ADDR + I2C_READ );        // set device address and read mode
	        ret = i2c_readNak();                     
	        i2c_stop(); 

			itoa( ret, accel_data, 10);   // convert interger into string (decimal format)         
			//uart_puts( accel_data );
			//uart_putc( '\n' );

		}


	} /* end eval loop */
		
}