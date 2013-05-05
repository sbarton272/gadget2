#include "g2.h"

/* ==========================
 * ===== Init All ===========
 * ========================== */

/* Run all of the init functions */
void initSystem() {

	initLED();
	initUART();
    i2c_init();

    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();

    setLED(ON); // leave status light on

}


/* ==========================
 * ===== LED Controls =======
 * ========================== */

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


/* ==========================
 * ===== Serial Com =========
 * ========================== */

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
            return ERROR;
        }
        if ( *c & UART_BUFFER_OVERFLOW )
        {
            /* 
             * We are not reading the receive buffer fast enough,
             * one or more received character have been dropped 
             */
            uart_puts_P("Buffer overflow error: ");
            return ERROR;
        }
    }

    return OK;

}