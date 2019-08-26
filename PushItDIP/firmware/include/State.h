/*
 * State.h
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

class IGameEngine {
public:
	enum Event {
		// System Events
		NoEvent,
		Tick,

		// Button Events
		BtnMode,
		BtnStart,
		BtnRepeat,
		BtnYellow,
		BtnRed,
		BtnGreen,
		BtnBlue,

		Finished,
		Ok,
		NotOk,
		NextLevel,
		IsPlayer1,
		IsPlayer2,
	};

	enum Parameter {
		NoParam,

		Yellow,
		Red,
		Green,
		Blue,

		Player1,
		Player2,

		Lost,
		Won,

		CharC,
		CharP,
	};

	enum GameType {
		Independent,
		SinglePlayer,
		MultiPlayerHToH, // Head to Head
	};

	virtual void sendEvent(Event e) = 0;
};

class IState {
public:
	virtual void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param) {};
	virtual void exitAction(IGameEngine* gameEngine) {};
	virtual void tickAction(IGameEngine* gameEngine) {};
};