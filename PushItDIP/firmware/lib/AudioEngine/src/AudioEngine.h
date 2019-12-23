/*
 * AudioEngine.h
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

/*
 * Notes and Frequencies
 * From https://www.arduino.cc/en/Tutorial/ToneKeyboard
 */
/* Dummy notes we can't play, to match piano keyboard. */
#define TONE_A0  31
#define TONE_AS0  31

/* Lowest note tone() can play */
#define TONE_B0  31

#define TONE_C1  33
#define TONE_CS1 35
#define TONE_D1  37
#define TONE_DS1 39
#define TONE_E1  41
#define TONE_F1  44
#define TONE_FS1 46
#define TONE_G1  49
#define TONE_GS1 52
#define TONE_A1  55
#define TONE_AS1 58
#define TONE_B1  62

#define TONE_C2  65
#define TONE_CS2 69
#define TONE_D2  73
#define TONE_DS2 78
#define TONE_E2  82
#define TONE_F2  87
#define TONE_FS2 93
#define TONE_G2  98
#define TONE_GS2 104
#define TONE_A2  110
#define TONE_AS2 117
#define TONE_B2  123

#define TONE_C3  131 // Lowest note for PLAY command ("O1 C").
#define TONE_CS3 139
#define TONE_D3  147
#define TONE_DS3 156
#define TONE_E3  165
#define TONE_F3  175
#define TONE_FS3 185
#define TONE_G3  196
#define TONE_GS3 208
#define TONE_A3  220
#define TONE_AS3 233
#define TONE_B3  247

#define TONE_C4  262 // Middle C
#define TONE_CS4 277
#define TONE_D4  294
#define TONE_DS4 311
#define TONE_E4  330
#define TONE_F4  349
#define TONE_FS4 370
#define TONE_G4  392
#define TONE_GS4 415
#define TONE_A4  440
#define TONE_AS4 466
#define TONE_B4  494
#define TONE_C5  523

#define TONE_CS5 554
#define TONE_D5  587
#define TONE_DS5 622
#define TONE_E5  659
#define TONE_F5  698
#define TONE_FS5 740
#define TONE_G5  784
#define TONE_GS5 831
#define TONE_A5  880
#define TONE_AS5 932
#define TONE_B5  988

#define TONE_C6  1047
#define TONE_CS6 1109
#define TONE_D6  1175
#define TONE_DS6 1245
#define TONE_E6  1319
#define TONE_F6  1397
#define TONE_FS6 1480
#define TONE_G6  1568
#define TONE_GS6 1661
#define TONE_A6  1760
#define TONE_AS6 1865
#define TONE_B6  1976

#define TONE_C7  2093
#define TONE_CS7 2217
#define TONE_D7  2349
#define TONE_DS7 2489
#define TONE_E7  2637
#define TONE_F7  2794
#define TONE_FS7 2960
#define TONE_G7  3136
#define TONE_GS7 3322
#define TONE_A7  3520
#define TONE_AS7 3729
#define TONE_B7  3951
#define TONE_C8  4186

#define TONE_CS8 4435
#define TONE_D8  4699
#define TONE_DS8 4978

#define DEFAULT_OCTAVE 2
#define DEFAULT_VOLUME 15
#define DEFAULT_NOTELN 4
#define DEFAULT_TEMPO 2

class AudioEngine
{
public:
	AudioEngine();
	void begin(uint8_t buzzerPin);
	void update();
	bool isBusy();

	void playTone(unsigned int frequency, unsigned long duration = 0);
	void playMelody(const __FlashStringHelper* playString);
	void playMelody(const char* playString);

	void cancel();

private:
	void playWorker();
	void playNote(byte note, unsigned long duration);
	char getNextCommand(unsigned int* ptr, byte stringType);
	byte checkModifier(unsigned int* ptr, byte stringType, byte value);
	byte checkForVariableOrNumeric(unsigned int* ptr, byte stringType, char commandChar, byte value);

	void cancelTone();

	uint8_t buzzerPin;
	elapsedMillis curDuration;
	unsigned long maxDuration;

	bool tonePlaying;
	bool melodyPlaying;

	unsigned int melodyCommandPtr;
	byte melodyStringType;

	byte melodyOctave = DEFAULT_OCTAVE; // Octave (1-5, default 2)
	byte melodyVolume = DEFAULT_VOLUME; // Volume (1-31, default 15)
	byte melodyNoteLn = DEFAULT_NOTELN; // Note Length (1-255, default 4) - quarter note
	byte melodyTempo = DEFAULT_TEMPO; // Tempo (1-255, default 2)
};

extern AudioEngine Audio;