/*
 * GameData.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "GameData.h"
#include "LedEngine.h"
#include "AudioEngine.h"
#include "ModeHandler.h"
#include "Configuration.h"

GameDataClass GameData;

void GameDataClass::begin()
{
}

void GameDataClass::init()
{
	gameRound = 0;
	playRound = 0;
	randomSeed(millis());
}

void GameDataClass::addRandomMove()
{
	uint8_t newMove = random(0, 4); // min (included), max (exluded)
	gameBoard[gameRound++] = newMove;
	playRound = 0;
}

void GameDataClass::addMove(Colors color)
{
	gameBoard[gameRound++] = color;
	playRound = 0;
}

void GameDataClass::addKeyPress(Colors color)
{
	playBoard[playRound++] = color;
}

bool GameDataClass::playMoves(bool showGameRound)
{
	static uint8_t currentMove = 0;

	if (showGameRound) {
		SevenSegment.setNumber(gameRound);
	}

	playColor(gameBoard[currentMove]);

	if (currentMove == gameRound - 1) {
		currentMove = 0;
		return true;
	}

	currentMove++;
	return false;
}

void GameDataClass::playColor(Colors color)
{
	switch (color) {
	case Blue:
		Led.set(LedEngine::LedColor::Blue);
		Audio.playTone(TONE_GS5, getAudioSequenceLength());
		break;
	case Yellow:
		Led.set(LedEngine::LedColor::Yellow);
		Audio.playTone(TONE_DS5, getAudioSequenceLength());
		break;
	case Green:
		Led.set(LedEngine::LedColor::Green);
		Audio.playTone(TONE_CS5, getAudioSequenceLength());
		break;
	case Red:
		Led.set(LedEngine::LedColor::Red);
		Audio.playTone(TONE_CS6, getAudioSequenceLength());
		break;
	}
}

bool GameDataClass::isValidPlay()
{
	for (uint8_t i = 0; i < playRound; i++) {
		if (gameBoard[i] != playBoard[i]) {
			return false;
		}
	}
	return true;
}

bool GameDataClass::isLastButton()
{
	return gameRound == playRound;
}

bool GameDataClass::isLastLevel()
{
	return gameRound > Mode.getLevelCount();
}

void GameDataClass::saveCurrentGameBoard()
{
	Configuration.get().Play_MaxLevel = gameRound - 1;
	Configuration.get().Play_MaxMode = Mode.getMode();
	for (uint8_t i = 0; i < gameRound; i++) {
		Configuration.get().Play_MaxGameBoard[i] = gameBoard[i];
	}
	Configuration.write();
}

void GameDataClass::loadBestGameBoard()
{
	gameRound = Configuration.get().Play_MaxLevel + 1;
	Mode.setMode(Configuration.get().Play_MaxMode);
	for (uint8_t i = 0; i < gameRound; i++) {
		gameBoard[i] = Configuration.get().Play_MaxGameBoard[i];
	}
}

uint16_t GameDataClass::getAudioSequenceLength()
{
	// gameRound is always one step ahead
	if (gameRound - 1 >= 0 && gameRound - 1 < 5) {
		return 420;
	}
	else if (gameRound - 1 >= 5 && gameRound - 1 < 13) {
		return 320;
	}
	else if (gameRound - 1 >= 13) {
		return 220;
	}
}