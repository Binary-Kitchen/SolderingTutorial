/*
 * ModeHandler.h
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

#include "SevenSegment.h"
#include "State.h"

#define MODE_MIN 0
#define MODE_MAX 4

class ModeHandler
{
public:
	void begin();
	void cycle();
	void show();
	uint8_t getLevelCount();
	uint8_t getMode();
	void setMode(uint8_t mode);
	IGameEngine::GameType getGameType();

private:
	uint8_t currentMode = MODE_MIN;
};

extern ModeHandler Mode;