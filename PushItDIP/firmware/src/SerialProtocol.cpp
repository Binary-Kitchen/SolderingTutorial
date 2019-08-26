/*
 * SerialProtocol.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include "SerialProtocol.h"
#include "AudioEngine.h"
#include "SevenSegment.h"

SerialProtocolClass SerialProtocol;

#define MSG_HELLO	'h'
#define MSG_ACK		'a'
#define MSG_LOST	'l'
#define MSG_WON		'w'

SerialProtocolClass::SerialProtocolClass()
{
}

void SerialProtocolClass::begin()
{
	Serial.begin(9600);
}

void SerialProtocolClass::beginHandshake()
{
	flushBuffer();
	SevenSegment.setNumber(0);
	lastRun = 0;
	timeRunning = 0;
	playerNumber = 0;
	handshakePerformed = false;
}

void SerialProtocolClass::flushBuffer()
{
	while(Serial.available()) {
    	char t = Serial.read();
	}
}

uint8_t SerialProtocolClass::performHandshake()
{
	if (lastRun > 1000) {
		Audio.playTone(TONE_D1, 100);
		SevenSegment.setNumber(++timeRunning);
		Serial.write(MSG_HELLO);
		lastRun = 0;
	}

	if (Serial.available()) {
		uint8_t received = Serial.read();
		if (received == MSG_HELLO) {
			Serial.write(MSG_ACK);
			playerNumber = 2;
			handshakePerformed = true;
		}
		else if (received == MSG_ACK) {
			playerNumber = 1;
			handshakePerformed = true;
		}
	}

	return playerNumber;
}

// returns 0 if no data is received, 99 if other player lost and
// 1-4 for different colors
IGameEngine::Parameter SerialProtocolClass::receiveChoice()
{
	uint8_t choice = 0;

	if(Serial.available()) {
		choice = Serial.read();

		if (choice == MSG_LOST) {
			choice = IGameEngine::Parameter::Lost;
		}
		else if (choice == MSG_WON) {
			choice = IGameEngine::Parameter::Won;
		}
	}

	return choice;
}

void SerialProtocolClass::sendChoice(IGameEngine::Parameter choice)
{
	if (handshakePerformed) {
		Serial.write(choice);
	}
}

void SerialProtocolClass::sendLostGame()
{
	if (handshakePerformed) {
		Serial.write(MSG_LOST);
	}
}

void SerialProtocolClass::sendWonGame()
{
	if (handshakePerformed) {
		Serial.write(MSG_WON);
	}
}