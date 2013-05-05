#include "g2.h"

/* =================================================================
 * ===== Init All ==================================================
 * ================================================================= */

/* Run all of the init functions, blinks twice */
void initSystem() {

	initLED();
    setLED(ON);
    _delay_ms(500);
    setLED(OFF);
    _delay_ms(500);
	initUART();

    // now enable interrupt, since UART library is interrupt controlled
    sei();

    setLED(ON);
    _delay_ms(500);
    setLED(OFF);
    _delay_ms(500);
    
    initI2C();

    setLED(ON); // leave status light on

}


/* ================================================================
 * ===== LED Controls =============================================
 * ================================================================ */

/* In this function you should set the LED pins to be outputs and
   set them high to turn them off (because our LEDs are active low)
 */
void initLED()
{
	// Set LED pin direction to output
	DDRC |= _BV(LED);
	
}

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


/* ============================================================
 * ===== Serial Com ===========================================
 * ============================================================ */

 /* Activate the device to laptop serial
 */
void initUART()
{

	uart_init( UART_BAUD_SELECT( UART_BAUD_RATE, CPU_CLK ) );

}


/* get char from uart and check for error conditions
 * store to char if no error (returns status code)
 */
   
int safeUARTgetc( int* c ) {

	*c = uart_getc();

    if ( *c & UART_NO_DATA ) {
    	*c = 0;
    	return NOTHING; // no error, nothing to read
    }
    else
    {
        /*
         * new data available from UART
         * check for Frame or Overrun error
         */
        if ( *c & UART_FRAME_ERROR )
        {
            /* Framing Error detected, i.e no stop bit detected */
            uart_puts_P("UART Frame Error: ");
            _delay_ms(ERROR_DELAY);

            return ERROR;
        }
        if ( *c & UART_OVERRUN_ERROR )
        {
            /* 
             * Overrun, a character already present in the UART UDR register was 
             * not read by the interrupt handler before the next character arrived,
             * one or more received characters have been dropped
             */
            uart_puts_P("UART Overrun Error: ");
            _delay_ms(ERROR_DELAY);

            return ERROR;
        }
        if ( *c & UART_BUFFER_OVERFLOW )
        {
            /* 
             * We are not reading the receive buffer fast enough,
             * one or more received character have been dropped 
             */
            uart_puts_P("Buffer overflow error: ");
            _delay_ms(ERROR_DELAY);

            return ERROR;
        }
    }

    return OK;

}


/* ============================================================
 * ===== I2C, accelerometer ===================================
 * ============================================================ */

 /* Activate the accelerometer */
void initI2C() {

    // validate values written
    unsigned char mode;
    char str[7];

    i2c_init();

    if ( !i2cWrite( MODE, 0x00) )
        uart_putc('E'); // Setting up MODE to Stand by to set SR

    if ( !i2cWrite( SR, 0x00) )
        uart_putc('E'); // Setting up SR register to 120 samples active and auto sleep mode

    if ( !i2cWrite( MODE, 0x01) )
        uart_putc('E'); //Setting up MODE Active to START measures 

    mode = i2cRead(MODE);

    itoa( mode, str, 10);   // convert interger into string (decimal format)         
    uart_puts_P( "Accelerometer online: ");
    uart_puts( str );
    uart_putc( '\n' );

}


/* wrapper for i2c_start to catch errors and print */
int i2cSafeStart( unsigned char addr ) {

    // set device in active mode
    if ( i2c_start( addr ) ) {

        // error, so stop
        i2c_stop();

        uart_puts_P("Accelerometer unavailable\n");
        _delay_ms(ERROR_DELAY);

        return 0; // error condition

    } else {

        return 1; // ok
    }


}

/* write register value, one byte reg value, one byte data 
   returns 0 on Error, 1 on success
 */
int i2cWrite( unsigned char reg, unsigned char data) {

    if ( i2cSafeStart( ACCEL_ADDR + I2C_WRITE ) ) {

        if ( i2c_write( reg ) ) {
            i2c_stop();
            return 0;
        }

        if ( i2c_write( data ) ) {
            i2c_stop();
            return 0;
        }

        i2c_stop(); 

        return 1;
    }

    return 0;
}

/* read register value, one byte reg value, one byte data
    returns ERROR status on error condition
 */
unsigned char i2cRead( unsigned char reg) {

    unsigned char ret = 0;

    if ( i2cSafeStart( ACCEL_ADDR + I2C_WRITE ) ) {

        i2c_write( reg );
        i2c_rep_start( ACCEL_ADDR + I2C_READ );        // set device address and read mode
        ret = i2c_readNak(); 
        i2c_stop(); 

        return ret;
    
    }

    return (unsigned char)ERROR;
}


/* read X, Y, Z data and store in struct
 */
int i2cReadXYZ( struct accel_data_t *accel_data ) {

    if ( i2cSafeStart( ACCEL_ADDR + I2C_WRITE ) ) {

        i2c_write( XOUT );
        i2c_rep_start( ACCEL_ADDR + I2C_READ );        // set device address and read mode
        accel_data->X = i2c_readAck(); 
        accel_data->Y = i2c_readAck(); 
        accel_data->Z = i2c_readNak(); 
        i2c_stop(); 

        return OK;
    
    }

    return ERROR;
}


/* print X,Y,Z data to the laptop */
void printXYZ( struct accel_data_t accel_data ) {

    char X[7], Y[7], Z[7];

    itoa( accel_data.X, X, 16);
    itoa( accel_data.Y, Y, 16);
    itoa( accel_data.Z, Z, 16);
    

    uart_puts( "X: " );
    uart_puts( X );
    uart_puts( "; Y: " );
    uart_puts( Y );
    uart_puts( "; Z: " );
    uart_puts( Z );
    uart_putc( '\n' );

}

