/*
 * GameData.h
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

#define MAX_ROUNDS_TO_WIN 31 // 0 - 30

class GameDataClass
{
public:
	enum Colors {
		Yellow = 0,
		Red = 1,
		Green = 2,
		Blue = 3,
	};

	void begin();
	void init();
	void addRandomMove();
	void addMove(Colors color);
	void addKeyPress(Colors color);
	bool isValidPlay();
	bool isLastButton();
	bool isLastLevel();
	bool playMoves(bool showGameRound = true);
	void playColor(Colors color);
	void saveCurrentGameBoard();
	void loadBestGameBoard();

private:
	uint8_t gameRound = 0;
	uint8_t playRound = 0;
	uint8_t gameBoard[MAX_ROUNDS_TO_WIN];
	uint8_t playBoard[MAX_ROUNDS_TO_WIN];

	uint16_t getAudioSequenceLength();
};

extern GameDataClass GameData;