/*
 * main.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include <avr/wdt.h>
#include "SevenSegment.h"
#include "ModeHandler.h"
#include "AudioEngine.h"
#include "LedEngine.h"
#include "GameEngine.h"
#include "GameData.h"
#include "Bounce2.h"
#include "Configuration.h"
#include "SerialProtocol.h"

// Pin definitions
#define SEVEN_SEGMENT_CLK_PIN  15
#define SEVEN_SEGMENT_DATA_PIN 14
#define SEVEN_SEGMENT_STO_PIN  16
#define BUZZER_PIN             10
#define LED_YELLOW_PIN         2
#define LED_RED_PIN            3
#define LED_GREEN_PIN          4
#define LED_BLUE_PIN           5
#define BTN_YELLOW_PIN         6
#define BTN_RED_PIN            7
#define BTN_GREEN_PIN          8
#define BTN_BLUE_PIN           9
#define BTN_MODE_PIN           19
#define BTN_START_PIN          17
#define BTN_REPEAT_PIN         18

// Button position in BTN_PINS array
#define BTN_YELLOW 0
#define BTN_RED    1
#define BTN_GREEN  2
#define BTN_BLUE   3
#define BTN_MODE   4
#define BTN_START  5
#define BTN_REPEAT 6

const uint8_t btnPins[] = {
	BTN_YELLOW_PIN,
	BTN_RED_PIN,
	BTN_GREEN_PIN,
	BTN_BLUE_PIN,
	BTN_MODE_PIN,
	BTN_START_PIN,
	BTN_REPEAT_PIN
};

GameEngine game;
Bounce *buttons = new Bounce[sizeof(btnPins)];

void setup() {
	// Reset watchdog (must happen within the first few ticks)
	MCUSR = 0;
	wdt_disable();

	// Initialize 7 segment display
	SevenSegment.begin(
		SEVEN_SEGMENT_CLK_PIN, SEVEN_SEGMENT_DATA_PIN, SEVEN_SEGMENT_STO_PIN);
	SevenSegment.blank();
	Mode.begin();
	GameData.begin();
	SerialProtocol.begin();

	// Initialize LED pins
	Led.begin(
		LED_YELLOW_PIN, LED_RED_PIN,
		LED_GREEN_PIN, LED_BLUE_PIN);

	// Initialize buzzer
	Audio.begin(BUZZER_PIN);

	// Initialize button debouncing
	for (uint8_t btn = 0; btn < sizeof(btnPins); btn++) {
		buttons[btn].attach(btnPins[btn], INPUT_PULLUP);
		buttons[btn].interval(25);
	}

	// Initialize configuration
	Configuration.begin();

	// Initialize game engine
	game.begin();

	// Enable Pin change interrupt (required to wakeup after sleep)
	*digitalPinToPCMSK(BTN_START_PIN) |= _BV(digitalPinToPCMSKbit(BTN_START_PIN));
	PCICR |= _BV(digitalPinToPCICRbit(BTN_START_PIN));

	// Enable the watchdog
	wdt_enable(WDTO_60MS);
}

void loop() {

	wdt_reset();

	// Update button state
	// Handles debouncing and send button event to game engine
	for (uint8_t btn = 0; btn < sizeof(btnPins); btn++) {
		buttons[btn].update();

		if (buttons[btn].fell()) {
			switch (btn) {
			case BTN_YELLOW: game.sendEvent(IGameEngine::Event::BtnYellow); break;
			case BTN_RED: game.sendEvent(IGameEngine::Event::BtnRed); break;
			case BTN_GREEN: game.sendEvent(IGameEngine::Event::BtnGreen); break;
			case BTN_BLUE: game.sendEvent(IGameEngine::Event::BtnBlue); break;
			case BTN_MODE: game.sendEvent(IGameEngine::Event::BtnMode); break;
			case BTN_START: game.sendEvent(IGameEngine::Event::BtnStart); break;
			case BTN_REPEAT: game.sendEvent(IGameEngine::Event::BtnRepeat); break;
			}
		}
	}

	// Update AudioEngine
	Audio.update();

	// Update GameEngine
	game.sendEvent(GameEngine::Event::Tick);
	game.update();
}

ISR(PCINT1_vect)
{
}