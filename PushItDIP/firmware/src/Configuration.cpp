/*
 * Configuration.cpp
 *
 * Copyright (c) Thomas Basler, 2018
 *
 * Authors:
 *   Thomas Basler <thomas@familie-basler.net>
 *
 * This work is licensed under the terms of the GNU GPL, version 3.  See
 * the LICENSE file in the top-level directory.
 */

#include <EEPROM.h>
#include "Configuration.h"

#define CONFIG_VERSION 1
#define CONFIG_ID "42!"
#define CONFIG_START 32

CONFIG_T config;
ConfigurationClass Configuration;

void ConfigurationClass::begin()
{
	memset(&config, 0x0, sizeof(config));

	strlcpy(config.Cfg_Id, CONFIG_ID, sizeof(config.Cfg_Id));
	config.Cfg_Version = CONFIG_VERSION;

	if (!read()) {
		// Write default values
		write();
	}
	if (config.Cfg_Version != CONFIG_VERSION) {
		// Upgrade config
		merge();
	}
}

bool ConfigurationClass::write()
{
	strlcpy(config.Cfg_Id, CONFIG_ID, sizeof(config.Cfg_Id));
	config.Cfg_Version = CONFIG_VERSION;

	for (unsigned int i = 0; i < sizeof(config); i++) {
		if (EEPROM.read(CONFIG_START + i) != *((uint8_t*)&config + i)) {
			EEPROM.write(CONFIG_START + i, *((uint8_t*)&config + i));
		}
	}
	return true;
}

bool ConfigurationClass::read()
{
	if (EEPROM.read(CONFIG_START + 0) == CONFIG_ID[0] &&
		EEPROM.read(CONFIG_START + 1) == CONFIG_ID[1] &&
		EEPROM.read(CONFIG_START + 2) == CONFIG_ID[2]) {

		for (unsigned int i = 0; i < sizeof(config); i++) {
			*((uint8_t*)&config + i) = EEPROM.read(CONFIG_START + i);
		}
		return true;
	}
	else {
		return false;
	}
}

CONFIG_T& ConfigurationClass::get()
{
	return config;
}

void ConfigurationClass::merge()
{
	config.Cfg_Version = CONFIG_VERSION;
	write();
}