#include "g2.h"

/* =================================================================
 * ===== Init All ==================================================
 * ================================================================= */

/* Run all of the init functions */
void initSystem() {

	initLED();
	initUART();
    initI2C();

    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();

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
	
	// Start LEDs ON to signal device is on
	setLED(ON);
	
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

    uart_putc( '1' );

    i2c_init();

    uart_putc( '2' );

    if ( i2cSafeStart( ACCEL_ADDR + I2C_WRITE ) ) {
        
        uart_putc( '3' );

        i2cWrite( MODE, 0x00); // Setting up MODE to Stand by to set SR

        uart_putc( '4' );

        i2cWrite( SR, 0x00);  // Setting up SR register to 120 samples active and auto sleep mode

        uart_putc( '5' );

        i2cWrite( MODE, 0x01); //Setting up MODE Active to START measures 

        uart_putc( '6' );

        mode = i2cRead(MODE);

        uart_putc( '7' );

        itoa( mode, str, 10);   // convert interger into string (decimal format)         
        uart_puts_P( "Accelerometer online: ");
        uart_puts( str );
        uart_putc( '\n' );
    
    } else {

        uart_puts_P( "Accelerometer offline\n");

    }


}


/* wrapper for i2c_start to catch errors and print */
int i2cSafeStart( unsigned char addr ) {

    // set device in active mode
    if ( i2c_start( ACCEL_ADDR + I2C_WRITE ) ) {

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
   silent failure
 */
void i2cWrite( unsigned char reg, unsigned char data) {

    if ( i2cSafeStart( ACCEL_ADDR + I2C_WRITE ) ) {

        i2c_write( reg );
        i2c_write( data );
        i2c_stop(); 

    }
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




