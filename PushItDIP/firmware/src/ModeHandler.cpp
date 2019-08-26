/*
 * ModeHandler.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "ModeHandler.h"

ModeHandler Mode;

void ModeHandler::begin()
{
	SevenSegment.setNumber(currentMode);
}

void ModeHandler::show()
{
	SevenSegment.setNumber(currentMode);
}

void ModeHandler::cycle()
{
	if (currentMode == MODE_MAX) {
		currentMode = MODE_MIN;
	}
	else {
		currentMode++;
	}
	SevenSegment.setNumber(currentMode);
}

uint8_t ModeHandler::getLevelCount()
{
	// LevelCount is zero based
	switch (currentMode) {
	case 0: return 7;
	case 1: return 13;
	case 2: return 20;
	case 3: return 30;
	case 4: return 30;
	default: return 0;
	}
}

uint8_t ModeHandler::getMode()
{
	return currentMode;
}

void ModeHandler::setMode(uint8_t mode)
{
	currentMode = mode;
}

IGameEngine::GameType ModeHandler::getGameType()
{
	switch (currentMode) {
	case 0: return IGameEngine::GameType::SinglePlayer;
	case 1: return IGameEngine::GameType::SinglePlayer;
	case 2: return IGameEngine::GameType::SinglePlayer;
	case 3: return IGameEngine::GameType::SinglePlayer;
	case 4: return IGameEngine::GameType::MultiPlayerHToH;
	default: return IGameEngine::GameType::SinglePlayer;
	}
}