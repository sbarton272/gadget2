#include "g1.h"

volatile uint8_t led = OFF;
uint8_t buzzer;


/* ==========================
 * ===== Init Functions =====
 * ========================== */

/* In this function you should set the LED pins to be outputs and
   set them high to turn them off (because our LEDs are active low)
   Lastly, you should initialize timer 0's interrupts and prescalar
   to enable software PWM  */
void initLED()
{
	// Set LED pin direction to output
	DDRA |= _BV(LED);

	// Start LEDs ON to signal device is on
	setLED(ON);

}

/*  In this function you should set the button pin to be an input */
void initButton()
{
	// Set button as input
	DDRA &= ~_BV(BUTTON);
	// Enable pin change 0 interrupt
	GIMSK  |= _BV(PCIE0);
	// enable PCINT1 in the pin change mask, the button
	PCMSK0 |= _BV(BUTTON_PC);

}

/*  Set buzzer as output */
void initBuzzer()
{

	DDRA |= _BV(BUZZER); // buzzer output

	// Initialize timer 0 with a prescalar of 0 to turn off
	TCCR0B = 0;

	// Init with toggle and CTC mode
	TCCR0A = _BV(COM0B0) | _BV(WGM01);

	// init freq as 0
	OCR0A = FREQ;

}

/*  Set sharp ir as input */
void initSharpIR()
{

	DDRA &= ~_BV(SHARPIR);

}

void initADC()
{

	ADCSRA = _BV(ADEN);	// enable ADC

	// In this function you should set up and enable the ADC with the following characteristics:
	// Left adjust the result so that you can return 8-bit results easily
	// Set the ADC prescalar to a division factor of 64 so that the ADC clock is 8 MHz/64 = 125 KHz
	ADCSRB |= _BV(ADLAR);				// turns on 8-bit mode
	ADCSRB |= _BV(ADPS2) | _BV(ADPS1); 	// divide clock by 64 (ADPS2:0 set to 'b110)

	ADMUX = 0; // set read pin 0
	ADMUX |= _BV(REFS1); // set 1.1V as reference

}

// You do not need to modify this function
void initSystem()
{
	sleep_status = OFF;
	initBuzzer();
	initLED();  // Turn on our LED
	initButton(); // Turn on button interrupts
	initADC();
	sei();        // Nothing's gonna interrupt until we turn on global interrupts
}

/* ==========================
 * ===== LED Controls =======
 * ========================== */

/*  set the led value */
void setLED(uint8_t led)
{
	if(led) {
		PORTA |= _BV(LED);
	}
	else {
		PORTA &= ~_BV(LED);
	}
}

/* ==========================
 * ===== Button Controls ====
 * ===== And Sleep Mode =====
 * ========================== */

/*  In this function, return a non-zero value if the button was pressed, and 0
    if it is not pressed. */
int buttonPressed()
{
	return !!(PINA & _BV(BUTTON));
}

/* Pin change interrupt. Set the sleep_status to enable sleep if button pressed */
ISR(PCINT0_vect)
{
	sleep_status = ON;
}

/* place gadget in sleep mode, adctivated with button */
void goToSleep(void)
{

	// Delay for button debouncing
	_delay_ms(1000);

	// Set the sleep mode (could be done in the initSystem as well - only needs to be done once)
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// Set outputs to inputs to reduce power consumption
	DDRA &= ~_BV(LED); // LED

	// Allow stuff to trigger sleep mode but don't go to sleep yet
	sleep_enable();

	// Turn off brown-out detect
	sleep_bod_disable();

	// Send the sleep instruction
    sleep_cpu();

	// When we get here we've just woken up again, so disable the ability to sleep - brown-out detect automatically comes back
    sleep_disable();

	// Delay for a bit so that you don't accidentally go to sleep
	_delay_ms(1000);

	// Set LED pins back on
	initLED();

	// Make it so the button can send us back to sleep (set sleep_status to 0)
	sleep_status = OFF;

}

/* ==========================
 * ===== Buzzer Controls ====
 * ========================== */

void playBuzzer(uint8_t freq, uint8_t duration)
{
	// Set prescalar to 1024
	TCCR0B |= _BV(CS02) | _BV(CS00);

	uint8_t dur = duration / 3;

	OCR0A = 3;
	_delay_ms(dur);
	OCR0A = 4;
	_delay_ms(dur);
	OCR0A = 5;
	_delay_ms(dur);

	// Turn off the prescalar to turn off buzzer
	TCCR0B &= ~_BV(CS02) & ~_BV(CS01) & ~_BV(CS00);
	OCR0A = 0;
}

/* ==========================
 * ===== IR Controls ========
 * ========================== */

/*  In this function, return a non-zero value if the button was pressed, and 0
    if it is not pressed. */
int irDetected()
{
	return !!(PINA & _BV(SHARPIR));
}

uint8_t readIR()
{
	// Four lines:
	// Set analog input to your channel - hint, use regular =, not | or &
    // start a conversion, 
	// wait for the conversion to complete, 
	// and then return the result

	ADMUX = (ADMUX & 0xC0) | SHARPIR; // keep the reference the same, use IR channel
	ADCSRA |= _BV(ADSC); // start conversion

	// wait for conversion, wait for interupt flag
	while( !(ADCSRA & _BV(ADIF)) );

	ADCSRA |= _BV(ADIF); // set flag to 1, turns off flag

	return (uint8_t) ADCH;	// because taking 8 bit
}