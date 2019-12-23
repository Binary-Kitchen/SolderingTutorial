/*
 * AudioEngine.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

/*---------------------------------------------------------------------------*/
/*
Sub-Etha Software's PLAY Parser
By Allen C. Huffman
www.subethasoftware.com

This is an implementation of the Microsoft BASIC "PLAY" command, based on the
6809 assembly version in the Tandy/Radio Shack TRS-80 Color Computer's
Extended Color BASIC.

2018-02-20 0.00 allenh - Project began.
2018-02-28 0.00 allenh - Initial framework.
2018-03-02 0.00 allenh - More work on PLAY and its options.
2018-03-03 0.00 allenh - Most things seem to work now.
2018-03-04 0.00 allenh - Supports Flash-strings. Tweaking debug output.
2018-03-05 0.00 allenh - Fixed issue with dotted notes.
2018-03-11 1.00 allenh - Merging standalone player with SirSound player.

TODO:
* DONE: Data needs to be moved to PROGMEM.
* Pause may be slightly off (or the demo song is just wrong).
* DONE: Need a "reset to defaults" command.
* DONE: Add support for PROGMEM strings.
* Set variable (2-digit name, numeric or string).
* Use variables to support Xvar$; and =var;

TOFIX:
* Dotted notes do not work like the real PLAY command, so really long notes
  do not work. They will max out at a note length of 255 (60/sec) which is
  4 seconds. Since only one byte is being used for the note length, anything
  longer currently is not supported. But it could be, if it needs to be.

NOTE
----
N (optional) followed by a letter from "A" to "G" or a number from 1 to 12.
When using letters, they can be optionally followed by "#" or "+" to make it
as sharp, or "-" to make it flat. When using numbers, they must be separated
by a ";" (semicolon).

      C  C# D  D# E  F  F# G  G# A  A# B  (sharps)
      1  2  3  4  5  6  7  8  9  10 11 12
      C  D- D  E- E  F  G- G  A- A  B- B  (flats)

Due to how the original PLAY command was coded by Microsoft, it also allows
sharps and flats that would normally not be allowed. For instance, E# is the
same as F, and F- is the same a E. Since notes are numbered 1-12, the code
did not allow C- or B#. This quirk is replicated in this implementation.

OCTAVE
------
"O" followed by a number from 1 to 5. Default is octave 2, which includes
middle C. (Supports modifiers.)

LENGTH
------
"L" followed by a number from 1 to 255, with an optional "." after it to
add an additional 1/2 of the specified length. i.e., L4 is a quarter note,
L4. is like L4+L8 (dotted quarter note). Default is 2. (Supports modifiers.)

L1 - whole note
L2 - 1/2 half node
L3 - dotted quarter note (L4.)
L4 - 1/4 quarter note
L8 - 1/8 eighth note
L16 - 1/16 note
L32 - 1/32 note
L64 - 1/64 note

TEMPO
-----
"T" followed by a number from 1-255. Default is 2. (Supports modifiers.)

VOLUME
------
"V" followed by a number from 1-31. Default is 15. (Supports modifiers.)

PAUSE
"P" followed by a number from 1-255.

SUBSTRINGS
----------
To be documented, since we will need a special method to load them befoe
we can use them.

Non-Standard Extensions
-----------------------
"Z" to reset back to default settings.

MODIFIERS
---------
Many items that accept numbers can also use a modifier instead. The
modifier will apply to whatever the last value was. Modifiers are:

* "+" increase value by 1.
* "-" decreate value by 1.
* ">" double value.
* "<" halve value.

For instance, if the octave is currently 1 (O1), using "O+" will make it
octave 2. If Tempo was currenlty 2 (T2), using "T>" would make it 4. If a
modifier causes the value to go out of allowed range, the command will fail
the same as if the out-of-range value was used.
*/
/*---------------------------------------------------------------------------*/

#include "AudioEngine.h"

AudioEngine Audio;

#define STRINGTYPE_RAM 0
#define STRINGTYPE_FLASH 1

const byte g_NoteJumpTable[7] PROGMEM = {
	/*   A,  B, C, D, E, F, G */
	/**/ 10, 12, 1, 3, 5, 6, 8
};

static const uint16_t g_ToneTable[] PROGMEM =
{
	/*
	* Dummy notes we can't play, to present normal keyboard range.
	*/
	TONE_A0, TONE_AS0,
	/*
	* Lowest note Arduino can produce:
	*/
	TONE_B0,
	/*
	* Not supported by PLAY command.
	*/
	TONE_C1, TONE_CS1, TONE_D1, TONE_DS1, TONE_E1, TONE_F1,
	TONE_FS1, TONE_G1, TONE_GS1, TONE_A1, TONE_AS1, TONE_B1,
	/*
	* Not supported by PLAY command.
	*/
	TONE_C2, TONE_CS2, TONE_D2, TONE_DS2, TONE_E2, TONE_F2,
	TONE_FS2, TONE_G2, TONE_GS2, TONE_A2, TONE_AS2, TONE_B2,
	/*
	* PLAY "O1":
	*/
	TONE_C3, TONE_CS3, TONE_D3, TONE_DS3, TONE_E3, TONE_F3,
	TONE_FS3, TONE_G3, TONE_GS3, TONE_A3, TONE_AS3, TONE_B3,
	/*
	* PLAY "O2" - Middle C is C4.
	*/
	TONE_C4, TONE_CS4, TONE_D4, TONE_DS4, TONE_E4, TONE_F4,
	TONE_FS4, TONE_G4, TONE_GS4, TONE_A4, TONE_AS4, TONE_B4,
	/*
	* PLAY "O3":
	*/
	TONE_C5, TONE_CS5, TONE_D5, TONE_DS5, TONE_E5, TONE_F5,
	TONE_FS5, TONE_G5, TONE_GS5, TONE_A5, TONE_AS5, TONE_B5,
	/*
	* PLAY "O4":
	*/
	TONE_C6, TONE_CS6, TONE_D6, TONE_DS6, TONE_E6, TONE_F6,
	TONE_FS6, TONE_G6, TONE_GS6, TONE_A6, TONE_AS6, TONE_B6,
	/*
	* PLAY "O5":
	*/
	TONE_C7, TONE_CS7, TONE_D7, TONE_DS7, TONE_E7, TONE_F7,
	TONE_FS7, TONE_G7, TONE_GS7, TONE_A7, TONE_AS7, TONE_B7,
	/*
	* Last few notes Arduino can produce:
	*/
	TONE_C8, TONE_CS8, TONE_D8, TONE_DS8
};

AudioEngine::AudioEngine() :
	maxDuration(0),
	tonePlaying(false),
	melodyPlaying(false)
{}

void AudioEngine::begin(uint8_t buzzerPin)
{
	this->buzzerPin = buzzerPin;
	pinMode(buzzerPin, OUTPUT);
}

void AudioEngine::update()
{
	if (tonePlaying && maxDuration > 0 && curDuration > maxDuration) {
		cancelTone();
	}
	if (melodyPlaying && !tonePlaying) {
		playWorker();
	}
}

bool AudioEngine::isBusy()
{
	return tonePlaying || melodyPlaying;
}

void AudioEngine::playTone(unsigned int frequency, unsigned long duration)
{
	tonePlaying = true; // Set this in every case, because silence has also a duration
	maxDuration = duration;
	if (frequency > 0) {
		::tone(buzzerPin, frequency);
	}
	else {
		::noTone(buzzerPin);
	}
	curDuration = 0;
}

void AudioEngine::cancel()
{
	cancelTone();
	melodyPlaying = false;
}

void AudioEngine::cancelTone()
{
	::noTone(buzzerPin);
	tonePlaying = false;
}

/*
 * playNote()
 *
 * Play the frequency for the specified note number.
 */
void AudioEngine::playNote(byte note, unsigned long duration)
{
	// Cease any note currently playing, first.
	cancelTone();

	if (note >= (sizeof(g_ToneTable) / sizeof(g_ToneTable[0]))) {
		return;
	}

	if (duration == 0) {
		duration = 1;
	}

	playTone(pgm_read_word_near(&g_ToneTable[note]), duration);
}

void AudioEngine::playMelody(const __FlashStringHelper* playString)
{
	melodyCommandPtr = (unsigned int)playString;
	melodyStringType = STRINGTYPE_FLASH;
	melodyPlaying = true;
}

void AudioEngine::playMelody(const char* playString)
{
	melodyCommandPtr = (unsigned int)playString;
	melodyStringType = STRINGTYPE_RAM;
	melodyPlaying = true;
}

void AudioEngine::playWorker()
{
	char commandChar;
	byte dotVal;
	byte dotDuration;
	byte noteDuration;
	byte value;
	byte note;

	if (melodyCommandPtr == 0) {
		return;
	}

	dotVal = 0; // Start out with no dotted value.

	// * GET NEXT COMMAND - RETURN VALUE IN ACCA
	commandChar = getNextCommand(&melodyCommandPtr, melodyStringType);

	switch (commandChar) {
	case '\"': // ignore quotes for testing.
		break;

	case '\0':
		value = 1; // no error
		melodyPlaying = false;
		break;

	case ';': // SUB COMMAND TERMINATED
		// IGNORE SEMICOLONS
		break;

	case '\'': // CHECK FOR APOSTROPHE
		// IGNORE THEM TOO
		break;

	case 'X': // CHECK FOR AN EXECUTABLE SUBSTRING
		// X - sub-string (x$; or xx$;)
		// process substring
		// Skip until semicolon or end of string.
		do {
			commandChar = getNextCommand(&melodyCommandPtr, melodyStringType);

			if (commandChar == '\0')
				break;
		} while (commandChar != ';');
		break;

	case 'O':
		// // ADJUST OCTAVE?
		// O - octave (1-5, default 2)
		//    Modifiers
		value = checkModifier(&melodyCommandPtr, melodyStringType, melodyOctave);
		if (value >= 1 && value <= 5) {
			melodyOctave = value;
		}
		else {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
		}
		break;

	case 'V':
		//  V - volume (1-31, default 15)
		//    Mofifiers
		value = checkModifier(&melodyCommandPtr, melodyStringType, melodyVolume);
		if (value >= 1 && value <= 31) {
			melodyVolume = value;
		}
		else {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
		}
		break;

	case 'L':
		//  L - note length
		//    Modifiers
		value = checkModifier(&melodyCommandPtr, melodyStringType, melodyNoteLn);
		if (value > 0) {
			melodyNoteLn = value;
		}
		else {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
			break;
		}
		//    . - dotted note
		dotVal = 0;
		while (1) {
			commandChar = getNextCommand(&melodyCommandPtr, melodyStringType);
			// Done if there is no more.
			if (commandChar == '\0') {
				value = 1; // no error
				melodyPlaying = false;
				break;
			} else if (commandChar == '.') {
				dotVal++;
			} else // Not a dot.
			{
				// Not a dot. Put it back.
				melodyCommandPtr--;
				break;
			}
		}
		break;

	case 'T':
		//  T - tempo (1-255, default 2)
		//    Modifiers
		value = checkModifier(&melodyCommandPtr, melodyStringType, melodyTempo);
		if (value > 0) {
			melodyTempo = value;
		} else {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
		}
		break;

	case 'P':
		//  P - pause (1-255)
		commandChar = getNextCommand(&melodyCommandPtr, melodyStringType);
		// Done if there is no more.
		if (commandChar == '\0') {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
			break;
		}

		// since =var; is not supported, we default to note length
		value = checkForVariableOrNumeric(&melodyCommandPtr, melodyStringType, commandChar, melodyNoteLn);
		if (value > 0) {
			// Create 60hz timing from Tempo and NoteLn (matching CoCo).
			noteDuration = (255 / value / melodyTempo);

			// Convert to 60/second
			// tm/60 = ms/1000
			// ms=(tm/60)*1000
			// no floating point needed this way
			// (tm*1000)/60
			//delay((noteDuration * 1000L) / 60L); // TODO: Replace
			playTone(0, (noteDuration * 1000L) / 60L);
		} else {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
		}
		break;

	case 'Z': // reset
		melodyOctave = DEFAULT_OCTAVE; // Octave (1-5, default 2)
		melodyVolume = DEFAULT_VOLUME; // Volume (1-31, default 15)
		melodyNoteLn = DEFAULT_NOTELN; // Note Length (1-255, default 4) - quarter note
		melodyTempo = DEFAULT_TEMPO; // Tempo (1-255, default 2)
		break;

	case 'N':
		//  N - note (optional)
		// Get next command character.
		commandChar = getNextCommand(&melodyCommandPtr, melodyStringType);

		// Done if there is no more.
		if (commandChar == '\0') {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
			break;
		}
		// Drop down to looking for note value.
		// no break

	default:
		// (A-G, 1-12)
		//    A-G
		note = 0;
		if (commandChar >= 'A' && commandChar <= 'G') {
			// Get numeric note value of letter note. (0-11)
			note = pgm_read_byte_near(&g_NoteJumpTable[commandChar - 'A']);
			// note is now 1-12

			// Check for sharp/flat character.
			commandChar = getNextCommand(&melodyCommandPtr, melodyStringType);

			// Done if there is no more.
			if (commandChar == '\0') {
				// Nothing to see after this one is done.
				value = 1; // no error
				melodyPlaying = false;
			}

			//      # - sharp
			//      + - sharp
			if (commandChar == '#' || commandChar == '+') // Sharp
			{
				note++; // Add one to note number (charp)
			} else if (commandChar == '-') // Flat
			{
				note--;
			} else {
				// Not a #, + or -. Put it back.
				melodyCommandPtr--;
			}
		} else // NOT A-G, check for 1-12
		{
			// L9BBE - Evaluate decimal expression in command string.
			// Jump to cmp '=' thing in modifier!
			note = checkForVariableOrNumeric(&melodyCommandPtr, melodyStringType, commandChar, 0);
			if (note == 0) {
				value = 0; // ?FC ERROR
				melodyPlaying = false;
				break;
			}
		}

		// L9B22 - Process note value.
		note--; // Adjust note number, BASIC uses 1-2, internally 0-11

		// If not was C (1), and was flat (0), this would make it 255.
		if (note > 11) {
			value = 0; // ?FC ERROR
			melodyPlaying = false;
			break;
		}

		/*--------------------------------------------------------*/
		// PROCESS NOTE HERE!
		/*--------------------------------------------------------*/

		// Convert tempo and length into note duration.
		noteDuration = (255 / melodyNoteLn / melodyTempo);

		// Add on dotted notes.
		if (dotVal != 0) {
			dotDuration = (noteDuration / 2);

			// Prevent note duration rollover since we currently do not
			// support a duration larger than 255. Would it be better to
			// just max out at 255 for the longest note possible?
			while ((dotVal > 0) && (noteDuration < (255 - dotDuration))) {
				noteDuration = noteDuration + dotDuration;
				dotVal--;
			}
		}

		// Convert from 60/second to ms
		// tm/60 = ms/1000
		// ms=(tm/60)*1000
		// no floating point needed this way
		// (tm*1000)/60
		unsigned long msDuration = ((noteDuration * 1000L) / 60L);

		// TonePlayer is based on 88-key piano keyboard. PLAY command
		// starts at the 27th note on a piano keyboard, so we add
		// that offset.
		playNote(27 + note + (12 * (melodyOctave - 1)), msDuration);
		break;
	}
}

char AudioEngine::getNextCommand(unsigned int* ptr, byte stringType)
{
	char commandChar;

	if (ptr == 0) {
		// Return nil character, and leave pointer alone.
		commandChar = '\0'; // NIL character
	}
	else {
		// Return next character, skipping spaces.
		while (1) {
			// Get character at current position.

			if (stringType == STRINGTYPE_RAM) {
				char* p = (char*)*ptr;
				commandChar = *p;
			}
			else {
				commandChar = pgm_read_byte_near(*ptr);
			}

			if (commandChar == '\0') {
				break;
			}

			// Increment pointer.
			(*ptr)++;

			if (commandChar != ' ') {
				break;
			}
		}
	}
	return commandChar;
}

byte AudioEngine::checkModifier(unsigned int* ptr, byte stringType, byte value)
{
	char commandChar;

	if (ptr != 0) {
		commandChar = getNextCommand(ptr, stringType);

		switch (commandChar) {
		case '\0':
			break;

		// ADD ONE?
		case '+':
			if (value < 255) {
				value++;
			}
			else {
				value = 0; // ?FC ERROR
			}
			break;

		// SUBTRACT ONE?
		case '-':
			if (value > 0) {
				value--;
			}
			else {
				value = 0; // ?FC ERROR
			}
			break;

		// MULTIPLY BY TWO?
		case '>':
			if (value <= 127) {
				value = value * 2;
			}
			else {
				value = 0; // ?FC ERROR
			}
			break;

		// DIVIDE BY TWO?
		case '<':
			if (value > 1) {
				value = value / 2;
			}
			else {
				value = 0; // ?FC ERROR
			}
			break;

		// Could be = or number, so we call a separate function since we
		// need this in the note routine as well.
		default:
			value = checkForVariableOrNumeric(ptr, stringType, commandChar, value);
			break;

		} // end of switch( commandChar )

	} // end of NULL check

	return value;
}

byte AudioEngine::checkForVariableOrNumeric(unsigned int* ptr, byte stringType, char commandChar, byte value)
{
    uint16_t temp; // MUL A*B = D

    switch (commandChar) {
    // SirSound will not have a way to support this.

    // CHECK FOR VARIABLE EQUATE
    case '=':
        // "=XX;" - value = whatever XX is set to.
        // Skip until semicolon or end of string.
        do {
            commandChar = getNextCommand(ptr, stringType);

            if (commandChar == '\0') {
				break;
			}
        } while (commandChar != ';');
        // Leave value unchanged, since we don't support it.

        //value = 0; // ?FC ERROR since we do not support this yet.
        break;

    // Else, check for numeric string.
    default:
        temp = 0;
        value = 0;
        do {
            // Stop at a non-numeric character.
            if (!isdigit(commandChar)) // not digit
            {
                // Rewind so we end up where we started.
                (*ptr)--;
                break;
            }

            // If here, it must be a digit, 0-9
            // Base 10. First time it will be 0 * 10.
            temp = temp * 10;

            // Convert ASCII number to value.
            temp = temp + (commandChar - '0');

            if (temp > 255) {
                temp = 0; // ?FC ERROR
                break;
			}

			// Get another command byte.
			commandChar = getNextCommand(ptr, stringType);

		} while (commandChar != '\0');

		value = temp;
		break;
	} // end of switch( commandChar )

	return value;
}