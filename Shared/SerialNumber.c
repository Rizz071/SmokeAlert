/*
 * SerialNumber.c
 *
 *  Created on: May 4, 2025
 *      Author: rizz
 */

#include "SerialNumber.h"
#include "stm32f1xx_hal.h"

SerialNumber_t get_serial_number() {

	SerialNumber_t Serial;

	Serial.byte_0 = HAL_GetUIDw0();
	Serial.byte_1 = HAL_GetUIDw1();
	Serial.byte_2 = HAL_GetUIDw2();

	return Serial;
}
