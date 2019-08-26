/*
 * GameEngine.h
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

#include "State.h"
#include "GameState.h"

#define EVENT_QUEUE_SIZE 4

class GameEngine : public IGameEngine
{
public:
	struct Transition {
		GameType    gameType;
		IState*     currentState;
		Event       event;
		uint16_t    maxTimeBeforeTransition;
		IState*     nextState;
		Parameter	parameter;
	};

	GameEngine();
	void begin();
	bool update();
	void sendEvent(Event e);

protected:
	inline bool evQueueIsEmpty() { return (evQueueHead == evQueueTail); }
	inline bool evQueueIsFull() { return ( ((evQueueTail + 1) & (evQueueSize - 1)) == evQueueHead ); }
	inline void evQueueFlush() { evQueueHead = 0; evQueueTail = 0; }

private:
	volatile Event evQueue[EVENT_QUEUE_SIZE];
	volatile uint8_t evQueueSize;
	volatile uint8_t evQueueHead;  // index begin data
	volatile uint8_t evQueueTail;  // index stop data

	// Independent States
	StateAttract			attract;
	StateMode				mode;
	StateRepeat				repeat;
	StateStartup			startup;
	StateLostGame			lostGame;
	StateWonGame			wonGame;
	StateInitGame			initGame;
	StatePlayMoves			playMoves;
	StateDummy				waitForButton;
	StatePressButton		pressButton;
	StateVerifyButton		verifyButton;
	StateDummy				waitBeforeLevel;

	// SinglePlayer States
	StateAddRandomMove		addRandomMove;

	// Multiplayer States
	StatePerformHandshake	performHandshake;
	StateShowPlayerNumber	showPlayer;
	StateWaitForChoice		waitForChoice;
	StateNotify			    notify;
	StateError				error;
	StateShowCharacter		chooseColor;
	StateChoiceButton		choiceButton;

	elapsedMillis timeInState;
	IState* currentState;
	IState* const initState;

	const Transition fsmApp[56] = {
	//						actualState						MaxTimeBeforeTransition		Parameter
	//	gameType								Event				Next State
		{Independent,		&startup,			Tick,		0,		&startup,			NoParam},	// Play startup sound
		{Independent,		&startup,			BtnRed,		0,		&attract,			NoParam},	// Allow skipping the startup sound by pressing the red key
		{Independent,		&startup,			Finished,	0,		&attract,			NoParam},	// Move forward to attract mode

		{Independent,		&attract,			Tick,		0,		&attract,			NoParam},	// Play attract animation
		{Independent,		&attract,			BtnMode,	0,		&mode,				NoParam},	// Handle press of Mode button
		{Independent,		&attract,			BtnRepeat,	0,		&repeat,			NoParam},	// Handle press of Repeat button
		{Independent,		&attract,			BtnStart,	0,		&initGame,			NoParam},	// Handle press of Start button

		{Independent,		&mode,				Finished,	0,		&attract,			NoParam},	// Cycle through modes

		{Independent,		&repeat,			Tick,		0,		&repeat,			NoParam},	// Play last won game
		{Independent,		&repeat,			Finished,	0,		&attract,			NoParam},	// Return to attract mode if finished

		{Independent,		&initGame,			Tick,		0,		&initGame,			NoParam},	// Play some animation before game
		{SinglePlayer,		&initGame,			Finished,	0,		&addRandomMove,		NoParam},	// Move on to first move in single player mode
		{MultiPlayerHToH,	&initGame,			Finished,	0,		&performHandshake,	NoParam},

		{SinglePlayer,		&addRandomMove,		Finished,	0,		&playMoves,			NoParam},	// Add random move in single player mode

		{Independent,		&playMoves,			Tick,		0,		&playMoves,			NoParam},	// Play all moves in memory
		{Independent,		&playMoves,			Finished,	0,		&waitForButton,		NoParam},	// Wait for user to press a button

		{SinglePlayer,		&waitForButton,		Tick,		3000,	&lostGame,			NoParam},	// Give user 3 seconds to press a button, otherwise game is lost
		{MultiPlayerHToH,	&waitForButton,		Tick,		3000,	&notify,			Lost},		// Give user 3 seconds to press a button, otherwise game is lost

		{Independent,		&waitForButton,		BtnBlue,	0,		&pressButton,		Blue},		// Player pressed blue button
		{Independent,		&waitForButton,		BtnYellow,	0,		&pressButton,		Yellow},	// Player pressed yellow button
		{Independent,		&waitForButton,		BtnRed,		0,		&pressButton,		Red},		// Player pressed red button
		{Independent,		&waitForButton,		BtnGreen,	0,		&pressButton,		Green},		// Player pressed green button

		{Independent,		&pressButton,		Tick,		0,		&pressButton,		NoParam},	// Light LED and play sound of pressed button
		{Independent,		&pressButton,		Finished,	0,		&verifyButton,		NoParam},	// Move on to verify pressed button

		{Independent,		&verifyButton,		Ok,			0,		&waitForButton,		NoParam},	// Pressed button ok, but not last button in level
		{SinglePlayer,		&verifyButton,		NotOk,		0,		&lostGame,			NoParam},	// Pressed button not ok --> player lost
		{MultiPlayerHToH,	&verifyButton,		NotOk,		0,		&notify,			Lost},		// Pressed button not ok, other player must be notified

		{Independent,		&verifyButton,		NextLevel,	0,		&waitBeforeLevel,	NoParam},	// Button ok and last button in level (Need some delay here)
		{SinglePlayer,		&verifyButton,		Finished,	0,		&wonGame,			NoParam},	// Button ok and last level reached --> player won
		{MultiPlayerHToH,	&verifyButton,		Finished,	0,		&notify,			Won},		// Notify other player that we won

		{SinglePlayer,		&waitBeforeLevel,	Tick,		500,	&addRandomMove,		NoParam},	// A short delay of 500ms before continue to next move
		{MultiPlayerHToH,	&waitBeforeLevel,	Tick,		500,	&chooseColor,		CharC},		// A short delay of 500ms before continue to choose color

		{MultiPlayerHToH,	&performHandshake,	Tick,		5000,	&error,				NoParam},	// Show error if no handshake was performed within 5 seconds
		{MultiPlayerHToH,	&performHandshake,	IsPlayer1,	0,		&showPlayer,		Player1},	// Show player number 1
		{MultiPlayerHToH,	&performHandshake,	IsPlayer2,	0,		&showPlayer,		Player2},	// Show player number 2

		{MultiPlayerHToH,	&showPlayer,		Tick,		0,		&showPlayer,		NoParam},	// Show player number
		{MultiPlayerHToH,	&showPlayer,		IsPlayer1,	0,		&chooseColor,		CharC},		// Let player 1 choose the first number for player 2
		{MultiPlayerHToH,	&showPlayer,		IsPlayer2,	0,		&waitForChoice,		NoParam},

		{MultiPlayerHToH,	&notify,			NotOk,		0,		&lostGame,			NoParam},	// Notify other player that we lost
		{MultiPlayerHToH,	&notify,			Ok,			0,		&wonGame,			NoParam},	// Notify other player that we won

		{MultiPlayerHToH,	&waitForChoice,		Tick,		30000,	&error,				NoParam},	// Wait 30 seconds for the other player, otherwise show error
		{MultiPlayerHToH,	&waitForChoice,		Ok,			0,		&playMoves,			NoParam},	// Other player added a color --> we have to play
		{MultiPlayerHToH,	&waitForChoice,		Finished,	0,		&wonGame,			NoParam},	// Other player lost --> we won
		{MultiPlayerHToH,	&waitForChoice,		NotOk,		0,		&lostGame,			NoParam},	// Other player won --> we lost

		{MultiPlayerHToH,	&chooseColor,		Tick,		10000,	&error,				NoParam},	// Wait 10 seconds for player to add a additional color
		{MultiPlayerHToH,	&chooseColor,		BtnBlue,	0,		&choiceButton,		Blue},		// Player pressed blue button
		{MultiPlayerHToH,	&chooseColor,		BtnYellow,	0,		&choiceButton,		Yellow},	// Player pressed yellow button
		{MultiPlayerHToH,	&chooseColor,		BtnRed,		0,		&choiceButton,		Red},		// Player pressed red button
		{MultiPlayerHToH,	&chooseColor,		BtnGreen,	0,		&choiceButton,		Green},		// Player pressed green button

		{MultiPlayerHToH,	&choiceButton,		Tick,		0,		&choiceButton,		NoParam},	// Light LED and play sound of pressed button
		{MultiPlayerHToH,	&choiceButton,		Finished,	0,		&waitForChoice,		NoParam},	// Lets wait for the other player

		{Independent,		&error,				Tick,		2000,	&attract,			NoParam},	// Indicate uart communication error

		{Independent,		&lostGame,			Tick,		0,		&lostGame,			NoParam},	// Play lost animation and sound
		{Independent,		&lostGame,			Finished,	0,		&attract,			NoParam},	// Move on to attract mode

		{Independent,		&wonGame,			Tick,		0,		&wonGame,			NoParam},	// Play won animation and sound
		{Independent,		&wonGame,			Finished,	0,		&attract,			NoParam},	// Move on to attract mode
	};
};