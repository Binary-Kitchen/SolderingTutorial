/*
 * DiceMC.c
 *
 * Created: 16.03.2018 23:00:21
 * Author : Thomas Basler
 */

#define F_CPU 1000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define BUTTON_PIN PB7

uint8_t DicePattern[7] = {
	0b00000000,		// 0
	0b00001000,		// 1
	0b00010100,		// 2
	0b00011100,		// 3
	0b01010101,		// 4
	0b01011101,		// 5
	0b01110111,		// 6
};

typedef enum {
	OFF,
	PLAY,
	SLOW_DOWN,
	FLASH,
	DONE
} modes;

modes current_mode = OFF;
unsigned long millis = 0;

int main(void)
{
	// Set PD0 - PD06 as output
	DDRD |= _BV(PD0) | _BV(PD1) | _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6);

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

	// Set Timer0 prescaler to 64
	TCCR0B |= _BV(CS00) | _BV(CS01);

	// Set Timer0 Mode to CTC
	TCCR0A |= _BV(WGM01);

	// Set Timer output compare
	// 1000000Hz / 64 / 16 = 1000Hz Timer CTC --> 1ms
	OCR0A = 16;

	// Enable interrupt on compare match
	TIMSK |= _BV(OCIE0A);

	// Enable interrupts
	sei();

	uint8_t current_val = 0;
	uint16_t current_slowdown_speed = 0;
	uint8_t slowdown_times = 0;
	uint8_t flash_state = 0;
	uint8_t flash_count = 0;
	uint32_t last_millis = 0;
	while(1) {
		switch (current_mode) {
		case OFF:
			// Turn Off LEDs
			PORTD = DicePattern[0];

			// Set Sleep mode to power down and enable sleep
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();

			// Perform sleep
			sleep_cpu();

			// Perform Wakeup
			sleep_disable();

			current_mode = PLAY;
			last_millis = millis;

			break;
		case PLAY:
			if (millis - last_millis > 50) {
				current_val = 1 + (rand() % 6);
				PORTD = DicePattern[current_val];
				last_millis = millis;
			}

			break;
		case SLOW_DOWN:
			if (millis - last_millis > current_slowdown_speed) {
				current_val = 1 + (rand() % 6);
				PORTD = DicePattern[current_val];
				current_slowdown_speed += 50;
				slowdown_times++;
				if (slowdown_times == 10) {
					current_mode = FLASH;
				}
				last_millis = millis;
			}
			break;
		case FLASH:
			if (millis - last_millis > 50) {
				if (flash_state == 0) {
					PORTD = DicePattern[current_val];
					flash_state = 1;
				}
				else {
					PORTD = DicePattern[0];
					flash_state = 0;
				}
				flash_count++;
				if (flash_count == 10) {
					PORTD = DicePattern[current_val];
					current_mode = DONE;
				}
				last_millis = millis;
			}
			break;
		case DONE:
			if (millis - last_millis > 10000) {
				current_mode = OFF;
			}
			break;
		}

		if (!bit_is_set(PINB, BUTTON_PIN)) {
			current_mode = PLAY;
		}
		else if (current_mode == PLAY) {
			current_mode = SLOW_DOWN;
			current_slowdown_speed = 50;
			slowdown_times = 0;
			flash_state = 0;
			flash_count = 0;
		}
	}
}

ISR(PCINT_vect)
{

}

ISR(TIMER0_COMPA_vect)
{
	millis++;
}
