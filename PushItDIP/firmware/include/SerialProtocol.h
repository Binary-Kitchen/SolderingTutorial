/*
 * SerialProtocol.h
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

#include "elapsedMillis.h"
#include "GameEngine.h"

class SerialProtocolClass
{
public:
	SerialProtocolClass();
	void begin();
	void beginHandshake();
	uint8_t performHandshake();
	IGameEngine::Parameter receiveChoice();
	void sendChoice(IGameEngine::Parameter choice);
	void sendLostGame();
	void sendWonGame();

private:

	void flushBuffer();

	elapsedMillis lastRun;
	uint8_t timeRunning;
	uint8_t playerNumber;
	bool handshakePerformed = false;
};

extern SerialProtocolClass SerialProtocol;