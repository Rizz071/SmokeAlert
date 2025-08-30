/*
 * esp32c3.c
 *
 *  Created on: Aug 22, 2025
 *      Author: rizz
 */

#include "main.h"
#include "FreeRTOS.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os2.h"
#include "esp32c3.h"
#include <stdbool.h>

static UART_HandleTypeDef *huart = NULL;

static osMessageQueueId_t esp32_uart_queueHandle;

void esp32_init(UART_HandleTypeDef *_huart,
		osMessageQueueId_t _esp32_uart_queueHandle);

bool esp32_send_AT(const char *command, uint8_t len);

void esp32_init(UART_HandleTypeDef *_huart,
		osMessageQueueId_t _esp32_uart_queueHandle) {

	huart = _huart;

	esp32_uart_queueHandle = _esp32_uart_queueHandle;

	HAL_UART_Transmit_IT(huart, (uint8_t*) "ATE0\r\n", 6);

}

bool esp32_send_AT(const char *cmd, uint8_t len) {

	uint8_t rx_buf[128] = { 0 };

	HAL_UART_Transmit_IT(huart, (uint8_t*) cmd, len - 1);

	HAL_UART_Receive_IT(huart, rx_buf, sizeof(rx_buf));

	// Задержка на обработку приёма данных.
	// Считаем, что за 50мс все данные успеют приёти точно.
	osDelay(50);

	// Останавливаем приём данныхз вручную,
	// чтобы в следующий раз начать запись в rx_buf сначала массива.
	HAL_UART_AbortReceive_IT(huart);

	if (strstr((char*) rx_buf, "OK") == NULL) {
		return false;
	}
	return true;

}

