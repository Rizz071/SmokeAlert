/*
 * SerialNumber.c
 *
 *  Created on: May 4, 2025
 *      Author: rizz
 */

#include "SerialNumber.h"
#include "stm32f1xx_hal.h"
#include "DebugLog.h"

SerialNumber_t get_serial_number() {
	debug("Polling for HW Serial...");

	SerialNumber_t Serial;

	Serial.byte_0 = HAL_GetUIDw0();
	Serial.byte_1 = HAL_GetUIDw1();
	Serial.byte_2 = HAL_GetUIDw2();

	debug("HW Serial: %d %d %d\n\r", Serial.byte_0, Serial.byte_1,
			Serial.byte_2);

	return Serial;
}
