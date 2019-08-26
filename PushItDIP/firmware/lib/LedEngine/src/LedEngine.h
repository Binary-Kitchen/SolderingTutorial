/*
 * LedEngine.h
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

class LedEngine
{
public:
	enum LedColor {
		None = 0,
		Yellow = _BV(0),
		Red = _BV(1),
		Green = _BV(2),
		Blue = _BV(3),
		All = _BV(0) | _BV(1) | _BV(2) | _BV(3),
	};

	void begin(uint8_t yellowPin, uint8_t redPin, uint8_t greenPin, uint8_t bluePin);
	void set(LedColor leds);
	void cycle();

private:
	uint8_t yellowPin;
	uint8_t redPin;
	uint8_t greenPin;
	uint8_t bluePin;
};

extern LedEngine Led;