/*
 * LedEngine.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "LedEngine.h"

LedEngine Led;

void LedEngine::begin(uint8_t yellowPin, uint8_t redPin, uint8_t greenPin, uint8_t bluePin)
{
	this->yellowPin = yellowPin;
	this->redPin = redPin;
	this->greenPin = greenPin;
	this->bluePin = bluePin;

	pinMode(yellowPin, OUTPUT);
	pinMode(redPin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
}

void LedEngine::set(LedColor leds)
{
	digitalWrite(yellowPin, (leds & LedColor::Yellow));
	digitalWrite(redPin, (leds & LedColor::Red));
	digitalWrite(greenPin, (leds & LedColor::Green));
	digitalWrite(bluePin, (leds & LedColor::Blue));
}

void LedEngine::cycle()
{
	static uint8_t led = 0;
	set(_BV(led));
	led++;
	led %= 4;
}