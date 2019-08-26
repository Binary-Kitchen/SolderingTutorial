/*
 * GameState.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "GameState.h"
#include "LedEngine.h"
#include "ModeHandler.h"
#include "AudioEngine.h"
#include "GameData.h"
#include "Configuration.h"
#include "SerialProtocol.h"

/*****************************************************************************/
/* Startup                                                                   */
/*****************************************************************************/
void StateStartup::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	lastRun = 0;
	Audio.playMelody(F("T5;C;E;F;L1;G;P4;L4;C;E;F;L1;G;P4;L4;C;E;F;L2;G;E;C;E;"
		"L1;D;P8;L4;E;E;D;L2.;C;L4;C;L2;E;L4;G;G;G;L1;F;L4;E;F;L2;G;E;L4;C;L8;"
		"D;D+;D;E;G;L4;A;L1;O3;C"));
}

void StateStartup::exitAction(IGameEngine* gameEngine)
{
	Audio.cancel();
	Led.set(LedEngine::LedColor::None);
}

void StateStartup::tickAction(IGameEngine* gameEngine)
{
	// Play startup animation
	if (lastRun > 500) {
		switch (ledColor) {
		case 0:
			Led.set(LedEngine::LedColor::Green | LedEngine::LedColor::Blue);
			break;
		case 1:
			Led.set(LedEngine::LedColor::Red | LedEngine::LedColor::Yellow);
			break;
		}
		// Toggle color
		ledColor ^= _BV(0);
		if (!Audio.isBusy()) {
			gameEngine->sendEvent(IGameEngine::Event::Finished);
		}
		lastRun = 0;
	}
}

/*****************************************************************************/
/* Attract                                                                   */
/*****************************************************************************/
void StateAttract::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	lastRun = 0;
	Mode.show();
}

void StateAttract::tickAction(IGameEngine* gameEngine)
{
	if (lastRun > 100) {
		Led.cycle();
		lastRun = 0;
	}
}

/*****************************************************************************/
/* Mode                                                                      */
/*****************************************************************************/
void StateMode::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	// Increment game mode here
	Mode.cycle();
	gameEngine->sendEvent(IGameEngine::Event::Finished);
}

/*****************************************************************************/
/* Repeat                                                                    */
/*****************************************************************************/
void StateRepeat::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	prevMode = Mode.getMode();
	GameData.loadBestGameBoard();
	Mode.show();
	playedLastColor = GameData.playMoves(false);
	lastRun = 0;
}

void StateRepeat::exitAction(IGameEngine* gameEngine)
{
	Mode.setMode(prevMode);
	// Turn LED Off
	Led.set(LedEngine::LedColor::None);
	// Turn Tone Off
	Audio.cancel();
}

void StateRepeat::tickAction(IGameEngine* gameEngine)
{
	if (Audio.isBusy()) {
		lastRun = 0;
	}
	if (lastRun > 50) { // This is the delay between each sound.
		if (playedLastColor) {
			gameEngine->sendEvent(IGameEngine::Event::Finished);
		}
		else {
			playedLastColor = GameData.playMoves(false);
			lastRun = 0;
		}
	}
}

/*****************************************************************************/
/* Init Game                                                                 */
/*****************************************************************************/
void StateInitGame::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	GameData.init();
	Led.set(LedEngine::LedColor::All);
	isOn = true;
	lastRun = 0;
}

void StateInitGame::tickAction(IGameEngine* gameEngine)
{
	if (isOn && lastRun > 1000) {
		Led.set(LedEngine::LedColor::None);
		isOn = false;
		lastRun = 0;
	}
	if (!isOn && lastRun > 250) {
		gameEngine->sendEvent(IGameEngine::Event::Finished);
	}
}

/*****************************************************************************/
/* Add Random Move                                                           */
/*****************************************************************************/
void StateAddRandomMove::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	GameData.addRandomMove();
	gameEngine->sendEvent(IGameEngine::Event::Finished);
}

/*****************************************************************************/
/* Play Moves                                                                */
/*****************************************************************************/
void StatePlayMoves::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	playedLastColor = GameData.playMoves();
	lastRun = 0;
}

void StatePlayMoves::exitAction(IGameEngine* gameEngine)
{
	// Turn LED Off
	Led.set(LedEngine::LedColor::None);
	// Turn Tone Off
	Audio.cancel();
}

void StatePlayMoves::tickAction(IGameEngine* gameEngine)
{
	if (Audio.isBusy()) {
		lastRun = 0;
	}
	if (lastRun > 50) { // This is the delay between each sound.
		if (playedLastColor) {
			gameEngine->sendEvent(IGameEngine::Event::Finished);
		}
		else {
			playedLastColor = GameData.playMoves();
			lastRun = 0;
		}
	}
}

/*****************************************************************************/
/* Press Button                                                              */
/*****************************************************************************/
void StatePressButton::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	switch (param) {
	case IGameEngine::Parameter::Blue:
		GameData.playColor(GameData.Colors::Blue);
		GameData.addKeyPress(GameData.Colors::Blue);
		break;
	case IGameEngine::Parameter::Yellow:
		GameData.playColor(GameData.Colors::Yellow);
		GameData.addKeyPress(GameData.Colors::Yellow);
		break;
	case IGameEngine::Parameter::Red:
		GameData.playColor(GameData.Colors::Red);
		GameData.addKeyPress(GameData.Colors::Red);
		break;
	case IGameEngine::Parameter::Green:
		GameData.playColor(GameData.Colors::Green);
		GameData.addKeyPress(GameData.Colors::Green);
		break;
	}

}

void StatePressButton::exitAction(IGameEngine* gameEngine)
{
	// Turn LED Off
	Led.set(LedEngine::LedColor::None);
	// Turn Tone Off
	Audio.cancel();
}

void StatePressButton::tickAction(IGameEngine* gameEngine)
{
	if (!Audio.isBusy()) {
		gameEngine->sendEvent(IGameEngine::Event::Finished);
	}
}

/*****************************************************************************/
/* Verify Button                                                             */
/*****************************************************************************/
void StateVerifyButton::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	if (GameData.isValidPlay()) {
		if (GameData.isLastButton()) {
			if (GameData.isLastLevel()) {
				// User won the whole game
				if (Mode.getGameType() == IGameEngine::GameType::SinglePlayer && Mode.getLevelCount() >= Configuration.get().Play_MaxLevel) {
					GameData.saveCurrentGameBoard();
				}
				gameEngine->sendEvent(IGameEngine::Event::Finished);
			}
			else {
				// User solved current level, switch to next level
				gameEngine->sendEvent(IGameEngine::Event::NextLevel);
			}
		}
		else {
			// User pressed the right button in the current level
			gameEngine->sendEvent(IGameEngine::Event::Ok);
		}
	}
	else {
		// User pressed the wrong button in the current level
		gameEngine->sendEvent(IGameEngine::Event::NotOk);
	}

}

/*****************************************************************************/
/* Lost Game                                                                 */
/*****************************************************************************/
void StateLostGame::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	currentState = 0;
	Led.set(LedEngine::LedColor::Red | LedEngine::LedColor::Green);
	Audio.playTone(TONE_G4, 750);
}

void StateLostGame::exitAction(IGameEngine* gameEngine)
{
	Audio.cancel();
	Led.set(LedEngine::LedColor::None);
}

void StateLostGame::tickAction(IGameEngine* gameEngine)
{
	if (!Audio.isBusy() && currentState == 0) {
		Audio.cancel();
		Led.set(LedEngine::LedColor::Blue | LedEngine::LedColor::Yellow);
		Audio.playTone(TONE_E4, 750);
		currentState = 1;
	}
	else if (!Audio.isBusy() && currentState == 1) {
		Audio.cancel();
		Led.set(LedEngine::LedColor::Red | LedEngine::LedColor::Green);
		Audio.playTone(TONE_D4, 750);
		currentState = 2;
	}
	else if (!Audio.isBusy() && currentState == 2) {
		Audio.cancel();
		Led.set(LedEngine::LedColor::Blue | LedEngine::LedColor::Yellow);
		Audio.playTone(TONE_C4, 750);
		currentState = 3;
	}
	else if (!Audio.isBusy() && currentState == 3) {
		gameEngine->sendEvent(IGameEngine::Event::Finished);
	}
}

/*****************************************************************************/
/* Won Game                                                                  */
/*****************************************************************************/
void StateWonGame::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	lastRun = 0;
	Audio.playMelody(F("ZO3ED8C8O2BGAO3E8O2B3P8O3E8D8C8O2B4G8E."
		"P2O3ED8C8O2BGG16AO3E8O2B3O3E8D8C8O2BG8E8EE8F"));
}

void StateWonGame::exitAction(IGameEngine* gameEngine)
{
	Audio.cancel();
	Led.set(LedEngine::LedColor::None);
}

void StateWonGame::tickAction(IGameEngine* gameEngine)
{
	// Play startup animation
	if (lastRun > 500) {
		switch (ledColor) {
		case 0:
			Led.set(LedEngine::LedColor::Red | LedEngine::LedColor::Yellow);
			break;
		case 1:
			Led.set(LedEngine::LedColor::Green | LedEngine::LedColor::Blue);
			break;
		}
		// Toggle color
		ledColor ^= _BV(0);
		if (!Audio.isBusy()) {
			gameEngine->sendEvent(IGameEngine::Event::Finished);
		}
		lastRun = 0;
	}
}

/*****************************************************************************/
/* Perform Handshake                                                         */
/*****************************************************************************/
void StatePerformHandshake::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	Led.set(LedEngine::LedColor::None);
	SerialProtocol.beginHandshake();
}

void StatePerformHandshake::exitAction(IGameEngine* gameEngine)
{
	// Turn Tone Off
	Audio.cancel();
}

void StatePerformHandshake::tickAction(IGameEngine* gameEngine)
{
	uint8_t result = SerialProtocol.performHandshake();
	if (result == 1) {
		gameEngine->sendEvent(IGameEngine::Event::IsPlayer1);
	}
	else if (result == 2) {
		gameEngine->sendEvent(IGameEngine::Event::IsPlayer2);
	}
}

/*****************************************************************************/
/* Show Player Number                                                        */
/*****************************************************************************/
void StateShowPlayerNumber::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	if (param == IGameEngine::Parameter::Player1) {
		playerNo = 1;
	} else {
		playerNo = 2;
	}
	SevenSegment.setChar('P');
	lastRun = 0;
}

void StateShowPlayerNumber::tickAction(IGameEngine* gameEngine)
{
	if (lastRun > 2000) {
		if (playerNo == 1) {
			gameEngine->sendEvent(IGameEngine::Event::IsPlayer1);
		} else {
			gameEngine->sendEvent(IGameEngine::Event::IsPlayer2);
		}
		return;
	}
	if (lastRun > 1000) {
		SevenSegment.setNumber(playerNo);
	}
}

void StateShowPlayerNumber::exitAction(IGameEngine* gameEngine)
{
	SevenSegment.blank();
}

/*****************************************************************************/
/* Wait for Input                                                            */
/*****************************************************************************/
void StateShowCharacter::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	switch (param) {
	case IGameEngine::Parameter::CharC:
		SevenSegment.setChar('C');
		break;
	case IGameEngine::Parameter::CharP:
		SevenSegment.setChar('P');
		break;
	}
}

void StateShowCharacter::exitAction(IGameEngine* gameEngine)
{
	SevenSegment.blank();
}

/*****************************************************************************/
/* Error                                                                     */
/*****************************************************************************/
void StateError::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	Led.set(LedEngine::LedColor::Red);
}

void StateError::exitAction(IGameEngine* gameEngine)
{
	Led.set(LedEngine::LedColor::None);
}

/*****************************************************************************/
/* Wait For Choice                                                           */
/*****************************************************************************/
void StateWaitForChoice::tickAction(IGameEngine* gameEngine)
{
	SevenSegment.playIdle();
	uint8_t rec = SerialProtocol.receiveChoice();

	// Received new move
	switch (rec) {
	case IGameEngine::Parameter::Yellow:
		GameData.addMove(GameData.Colors::Yellow);
		gameEngine->sendEvent(IGameEngine::Event::Ok);
		break;
	case IGameEngine::Parameter::Red:
		GameData.addMove(GameData.Colors::Red);
		gameEngine->sendEvent(IGameEngine::Event::Ok);
		break;
	case IGameEngine::Parameter::Green:
		GameData.addMove(GameData.Colors::Green);
		gameEngine->sendEvent(IGameEngine::Event::Ok);
		break;
	case IGameEngine::Parameter::Blue:
		GameData.addMove(GameData.Colors::Blue);
		gameEngine->sendEvent(IGameEngine::Event::Ok);
		break;
	case IGameEngine::Parameter::Lost:
		// Other player lost --> we won
		gameEngine->sendEvent(IGameEngine::Event::Finished);
		break;
	case IGameEngine::Parameter::Won:
		// Other player won --> we lost
		gameEngine->sendEvent(IGameEngine::Event::NotOk);
		break;
	}
}

void StateWaitForChoice::exitAction(IGameEngine* gameEngine)
{
	SevenSegment.blank();
}

/*****************************************************************************/
/* Notify                                                                    */
/*****************************************************************************/
void StateNotify::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	if (param == IGameEngine::Parameter::Lost) {
		SerialProtocol.sendLostGame();
		gameEngine->sendEvent(IGameEngine::Event::NotOk);
	}
	else {
		SerialProtocol.sendWonGame();
		gameEngine->sendEvent(IGameEngine::Event::Ok);
	}
}

/*****************************************************************************/
/* Choice Button                                                             */
/*****************************************************************************/
void StateChoiceButton::entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param)
{
	switch (param) {
	case IGameEngine::Parameter::Blue:
		GameData.addMove(GameData.Colors::Blue);
		GameData.playColor(GameData.Colors::Blue);
		SerialProtocol.sendChoice(IGameEngine::Parameter::Blue);
		break;
	case IGameEngine::Parameter::Yellow:
		GameData.addMove(GameData.Colors::Yellow);
		GameData.playColor(GameData.Colors::Yellow);
		SerialProtocol.sendChoice(IGameEngine::Parameter::Yellow);
		break;
	case IGameEngine::Parameter::Red:
		GameData.addMove(GameData.Colors::Red);
		GameData.playColor(GameData.Colors::Red);
		SerialProtocol.sendChoice(IGameEngine::Parameter::Red);
		break;
	case IGameEngine::Parameter::Green:
		GameData.addMove(GameData.Colors::Green);
		GameData.playColor(GameData.Colors::Green);
		SerialProtocol.sendChoice(IGameEngine::Parameter::Green);
		break;
	}
}

void StateChoiceButton::exitAction(IGameEngine* gameEngine)
{
	// Turn LED Off
	Led.set(LedEngine::LedColor::None);
	// Turn Tone Off
	Audio.cancel();
}

void StateChoiceButton::tickAction(IGameEngine* gameEngine)
{
	if (!Audio.isBusy()) {
		gameEngine->sendEvent(IGameEngine::Event::Finished);
	}
}