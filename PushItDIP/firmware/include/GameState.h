/*
 * GameState.h
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
#include "elapsedMillis.h"

class StateDummy : public IState {
};

class StateStartup : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
	uint8_t runCount = 0;
	uint8_t ledColor = 0;
};

class StateAttract : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
};

class StateMode : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
};

class StateRepeat : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
	bool playedLastColor = false;
	uint8_t prevMode = 0;
};

class StateInitGame : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
	bool isOn = true;
};

class StateAddRandomMove : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
};

class StatePlayMoves : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
	bool playedLastColor = false;
};

class StatePressButton : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
};

class StateVerifyButton : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
};

class StateLostGame : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
private:
	uint8_t currentState = 0;
};

class StateWonGame : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
	uint8_t runCount = 0;
	uint8_t ledColor = 0;
};

class StatePerformHandshake : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
};

class StateShowPlayerNumber : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
private:
	elapsedMillis lastRun;
	uint8_t playerNo;
};

class StateShowCharacter : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
};

class StateError : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
};

class StateWaitForChoice : public IState {
public:
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
};

class StateNotify : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
};

class StateChoiceButton : public IState {
public:
	void entryAction(IGameEngine* gameEngine, IGameEngine::Parameter param);
	void exitAction(IGameEngine* gameEngine);
	void tickAction(IGameEngine* gameEngine);
};