/*
 * Kitchenkopf.c
 *
 * Created: 04.03.2017 00:47:04
 * Author : Thomas Basler
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define COUNT_ROW 3
#define COUNT_COL 7
#define COUNT_LED (COUNT_ROW * COUNT_COL)
volatile uint8_t Pixels[COUNT_ROW * COUNT_COL];
volatile uint8_t CurRow = 0;
volatile uint8_t Status = 0;

volatile uint8_t w1, w2, col;

#define BUTTON_PIN PB7

void LedsAllOff();
void PixelsAllSet(uint8_t val);

int main(void)
{
    // Set PD0 - PD6 as output
	DDRD |= _BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6);

	// Set PB0 - PB2 as output
	DDRB |= _BV(PB0) | _BV(PB1) | _BV(PB2);

	// Set PB7 as input
	DDRB &= ~_BV(BUTTON_PIN);

	// Set Pull-Up for PB7
	PORTB |= _BV(BUTTON_PIN);

	// Disable Analog Comparator to reduce power consumption
	ACSR |= _BV(ACD);

	// Enable pin change interrupt
	GIMSK |= _BV(PCIE);

	// Enable pin change interrupt only on PB7
	PCMSK |= _BV(PCINT7);

	// Enable interrupts
	sei();

	// Set Sleep mode to power down and enable sleep
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// Perform sleep
	Status = 0;
	sleep_cpu();

	// Perform Wakeup
	sleep_disable();

	// Set Timer0 prescaler to 64
	TCCR0B |= _BV(CS00) | _BV(CS01);

	// Set Timer0 Mode to CTC
	TCCR0A |= _BV(WGM01);

	// Set Timer output compare
	// 1000000Hz / 64 / 52 = 300Hz Timer CTC --> 100Hz for the whole image (3 rows)
	OCR0A = 52;

	// Enable interrupt on compare match
	TIMSK |= _BV(OCIE0A);

	while (1) 
    {
		PixelsAllSet(0);

		// Animation 1
		for (uint8_t c = 0; c < 2; c++) {
			for (uint8_t i = 0; i < COUNT_LED; i++) {
				Pixels[i] ^= _BV(0);
				_delay_ms(50);
			}
		}

		// Animation 2
		for (uint8_t c = 0; c < 20; c++) {
			for (uint8_t i = 0; i < COUNT_LED; i+=2) {
				Pixels[i] |= _BV(0);
			}
			_delay_ms(10);
			PixelsAllSet(0);
			for (uint8_t i = 1; i < COUNT_LED; i+=2) {
				Pixels[i] |= _BV(0);
			}
			_delay_ms(10);
		}

		// Animation 3
		for (uint8_t c = 0; c < 10; c++) {
			PixelsAllSet(0);
			_delay_ms(300);
			PixelsAllSet(1);
			_delay_ms(300);
		}
		
		// Animation 4
		PixelsAllSet(0);
		#define SNAKE_SIZE 5
		for (uint8_t c = 0; c < 5; c++) {
			for (uint8_t i = 0; i < COUNT_LED + SNAKE_SIZE; i++) {
				if (i < COUNT_LED) {
					Pixels[i] |= _BV(0);
				}
				else {
					Pixels[i - COUNT_LED] |= _BV(0);
				}
				if (i >= SNAKE_SIZE) {
					Pixels[i - SNAKE_SIZE] &= ~_BV(0);
					_delay_ms(100);
				}
			}
		}

		// Animation 5
		for (uint8_t c = 0; c < 20; c++) {
			for (uint8_t i = 0; i < COUNT_LED / 2; i++) {
				PixelsAllSet(0);
				Pixels[i] |= _BV(0);
				Pixels[i + COUNT_LED / 2] = _BV(0);
				_delay_ms(50);
			}
		}

		// Animation 6
		for (uint8_t c = 0; c < 20; c++) {
			for (uint8_t i = COUNT_LED / 2; i > 0; i--) {
				PixelsAllSet(0);
				Pixels[i] |= _BV(0);
				Pixels[i + COUNT_LED / 2] = _BV(0);
				_delay_ms(50);
			}
		}
    }
}

inline void LedsAllOff()
{
	// Turn all off
	PORTD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6));
	PORTB |= _BV(PB0) | _BV(PB1) | _BV(PB2);
}

void PixelsAllSet(uint8_t val)
{
	for (uint8_t i = 0; i < COUNT_LED; i++) {
		Pixels[i] = val;
	}
}

ISR(PCINT_vect)
{
	// Check if pin is high --> Button released
	if (bit_is_set(PINB, BUTTON_PIN)) {
		return;
	}
	
	if (Status) {
		// Current Status is on, so turn it off

		// Debounce button press
		for (w1 = 0; w1 < 0x10; w1++) {
			for (w2 = 0; w2 < 254; w2++) {
				if (bit_is_set(PINB, BUTTON_PIN)) {
					return;
				}
			}
		}
		
		// Debounce button release
		for (w1 = 0; w1 < 254; w1++) {
			loop_until_bit_is_set(PINB, BUTTON_PIN);
		}

		// Clear Pin change interrupt (which was set when key was released)
		EIFR &= ~_BV(PCIF);

		// Enable interrupts to wakeup again
		sei();

		// Turn all off
		LedsAllOff();

		// Set Sleep mode to power down and enable sleep
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();

		// Perform sleep
		Status = 0;
		sleep_cpu();
	} 
	else {
		// Current Status is off, so turn it on

		// Perform Wakeup
		Status = 1;
		sleep_disable();
	}
}

ISR(TIMER0_COMPA_vect)
{
	// Turn all off
	LedsAllOff();

	// Activate current row
	PORTB &= ~_BV(CurRow);

	// Set Pixels of current row
	for (col = 0; col < COUNT_COL; col++) {
		if (Pixels[CurRow * COUNT_COL + col] & 1) {
			PORTD |= _BV(col);
		}
	}

	// Switch to next row
	CurRow++;
	if (CurRow >= COUNT_ROW) {
		CurRow = 0;
	}
}