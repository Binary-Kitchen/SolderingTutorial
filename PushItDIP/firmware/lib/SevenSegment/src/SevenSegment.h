/*
 * SevenSegment.h
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "elapsedMillis.h"

class SevenSegmentClass
{
public:
	SevenSegmentClass();
	void begin(uint8_t clkPin, uint8_t dataPin, uint8_t stoPin);
	void setNumber(uint8_t numToShow);
	void setChar(char charToShow);
	void blank();
	void playIdle();

private:
	uint8_t clkPin;
	uint8_t dataPin;
	uint8_t stoPin;

	elapsedMillis lastRunIdle = 0;
	uint8_t currentIdle = 0;
};

extern SevenSegmentClass SevenSegment;