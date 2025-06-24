/*
 * DataStorage.c
 *
 *  Created on: Jun 24, 2025
 *      Author: rizz
 */

#include "DataStorage.h"
#include "Types.h"
#include "main.h"
#include "DebugLog.h"
#include <string.h>
#include <stdbool.h>

#define FLASH_ADDR 		0x08007C00
#define MAGIC_VALUE 	0x1234ABCD

int storeSettingsToFlash(Settings_t *s) {
	debug("\tReading settings to FLASH...");

	HAL_FLASH_Unlock();

	// Стираем страницу
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError;

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = FLASH_ADDR;
	EraseInitStruct.NbPages = 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
		debug("ERROR: Can't erase memory page to save data!\n\r");
		return 1;
	}

	uint32_t alarm_level_uint32;
	memcpy(&alarm_level_uint32, &(s->alarm_level), sizeof(alarm_level_uint32));

	// Записываем структуру по 32-битам
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR + 0, MAGIC_VALUE)
			!= HAL_OK
			|| HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR + 4,
					alarm_level_uint32) != HAL_OK
			|| HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR + 8,
					s->sleep_time) != HAL_OK
			|| HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR + 12,
					s->times_to_heartbeat) != HAL_OK) {

		debug("ERROR: Failed to write settings to flash!\n\r");

		HAL_FLASH_Lock();

		return 2;
	}

	HAL_FLASH_Lock();

	debug("\t...DONE");

	return 0;
}

Settings_t retrieveSettingsFromFlash() {

	debug("\tReading settings from FLASH ");

	bool IS_SAVED_SATTING_FLAG = false;

	uint32_t magic;
	uint32_t alarm_level_uint32;

	Settings_t s;

	// Default parameters
	s.alarm_level = 0.8f;
	s.sleep_time = 4; // in sec
	s.times_to_heartbeat = 3; // in times

	magic = *(uint32_t*) FLASH_ADDR;

	if (magic == MAGIC_VALUE) {
		// Данные валидны

		IS_SAVED_SATTING_FLAG = true;

		alarm_level_uint32 = *(uint32_t*) (FLASH_ADDR + 4);
		s.sleep_time = *(uint32_t*) (FLASH_ADDR + 8);
		s.times_to_heartbeat = *(uint32_t*) (FLASH_ADDR + 12);

		memcpy(&(s.alarm_level), &alarm_level_uint32,
				sizeof(alarm_level_uint32));

	}

	if (!IS_SAVED_SATTING_FLAG) {
		debug("\t...using defaults\n\r");
	} else {
		debug("\t...found in flash\n\r");
	}

	return s;
}

