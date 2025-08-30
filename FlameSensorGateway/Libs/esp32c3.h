/*
 * esp32c3.h
 *
 *  Created on: Aug 22, 2025
 *      Author: rizz
 */

#ifndef ESP32C3_H_
#define ESP32C3_H_

#include "main.h"
#include <stdbool.h>

void esp32_init(UART_HandleTypeDef *_uart,
		osMessageQueueId_t _esp32_uart_queueHandle);

bool esp32_send_AT(const char *cmd, uint8_t len);

#endif /* ESP32C3_H_ */
