/*
 * DebugLog.c
 *
 *  Created on: May 17, 2025
 *      Author: rizz
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"

static UART_HandleTypeDef *huart;

void debug_init(UART_HandleTypeDef *hu) {
	huart = hu;
}

void debug(const char *format, ...) {

	char buffer[128];

	char msg[96];

	va_list args;
	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	snprintf(buffer, sizeof(buffer), "[%4lu ms] %s\n\r", HAL_GetTick(), msg);

	HAL_UART_Transmit(huart, (uint8_t*) buffer, strlen(buffer), HAL_MAX_DELAY);
}
