/*
 * Configuration.h
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

#include "GameData.h"

struct CONFIG_T {
	uint8_t Cfg_Id[4];
	uint8_t Cfg_Version;
	uint8_t Play_MaxMode;
	uint8_t Play_MaxLevel;
	uint8_t Play_MaxGameBoard[MAX_ROUNDS_TO_WIN];
};

class ConfigurationClass
{
public:
	void begin();
	bool read();
	bool write();
	void merge();
	CONFIG_T& get();
};

extern ConfigurationClass Configuration;