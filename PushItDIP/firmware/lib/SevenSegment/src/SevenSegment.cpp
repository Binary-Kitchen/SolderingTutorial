/*
 * SevenSegment.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "SevenSegment.h"

#define SEG_A  (_BV(0))
#define SEG_B  (_BV(2))
#define SEG_C  (_BV(4))
#define SEG_D  (_BV(7))
#define SEG_E  (_BV(5))
#define SEG_F  (_BV(3))
#define SEG_G  (_BV(1))
#define SEG_DP (_BV(6))

/*
        ***************
        *      A      *
        ***************
   *****               *****
   *   *               *   *
   * F *               * B *
   *   *               *   *
   *   *               *   *
   *****               *****
        ***************
        *      G      *
        ***************
   *****               *****
   *   *               *   *
   * E *               * C *
   *   *               *   *
   *   *               *   *
   *****               *****
        ***************     ******
        *      D      *     * DP *
        ***************     ******
*/

SevenSegmentClass SevenSegment;

static const byte digitCodeMap[] = {
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),  // 0
	(SEG_B | SEG_C),  // 1
	(SEG_A | SEG_B | SEG_G | SEG_E | SEG_D),  // 2
	(SEG_A | SEG_B | SEG_G | SEG_C | SEG_D),  // 3
	(SEG_F | SEG_G | SEG_B | SEG_C),  // 4
	(SEG_A | SEG_F | SEG_G | SEG_C | SEG_D),  // 5
	(SEG_A | SEG_F | SEG_G | SEG_C | SEG_D | SEG_E),  // 6
	(SEG_A | SEG_B | SEG_C),  // 7
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),  // 8
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)  // 9
};

static const byte idleMap[] = {
	SEG_A,
	SEG_F,
	SEG_G,
	SEG_E,
	SEG_D,
	SEG_C,
	SEG_G,
	SEG_B
};

SevenSegmentClass::SevenSegmentClass()
{}

void SevenSegmentClass::begin(uint8_t clkPin, uint8_t dataPin, uint8_t stoPin)
{
	this->clkPin = clkPin;
	this->dataPin = dataPin;
	this->stoPin = stoPin;
	pinMode(clkPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(stoPin, OUTPUT);
}

void SevenSegmentClass::setNumber(uint8_t numToShow)
{
	//ground latchPin and hold low for as long as you are transmitting
	digitalWrite(stoPin, LOW);
	if (numToShow <= 19) {
		shiftOut(dataPin, clkPin, MSBFIRST, ~(digitCodeMap[numToShow % 10] | (numToShow > 9 ? SEG_DP : 0)));
	}
	else if (numToShow <= 39) { // Show inverted if >19
		shiftOut(dataPin, clkPin, MSBFIRST, (digitCodeMap[numToShow % 10] | (numToShow > 29 ? 0 : SEG_DP)));
	}

	//return the latch pin high to signal chip that it
	//no longer needs to listen for information
	digitalWrite(stoPin, HIGH);
}

void SevenSegmentClass::setChar(char charToShow)
{
	//ground latchPin and hold low for as long as you are transmitting
	digitalWrite(stoPin, LOW);

	switch (charToShow) {
	case 'C':
		shiftOut(dataPin, clkPin, MSBFIRST, ~(SEG_A | SEG_F | SEG_E | SEG_D));
		break;
	case 'P':
		shiftOut(dataPin, clkPin, MSBFIRST, ~(SEG_A | SEG_B | SEG_G | SEG_F | SEG_E));
		break;
	default:
		shiftOut(dataPin, clkPin, MSBFIRST, ~(0));
	}

	//return the latch pin high to signal chip that it
	//no longer needs to listen for information
	digitalWrite(stoPin, HIGH);
}

void SevenSegmentClass::blank()
{
	//ground latchPin and hold low for as long as you are transmitting
	digitalWrite(stoPin, LOW);
	shiftOut(dataPin, clkPin, MSBFIRST, ~0);
	//return the latch pin high to signal chip that it
	//no longer needs to listen for information
	digitalWrite(stoPin, HIGH);
}

void SevenSegmentClass::playIdle()
{
	if (lastRunIdle > 100) {
		currentIdle++;
		currentIdle %= sizeof(idleMap);

		//ground latchPin and hold low for as long as you are transmitting
		digitalWrite(stoPin, LOW);

		shiftOut(dataPin, clkPin, MSBFIRST, ~(idleMap[currentIdle]));

		//return the latch pin high to signal chip that it
		//no longer needs to listen for information
		digitalWrite(stoPin, HIGH);

		lastRunIdle = 0;
	}
}