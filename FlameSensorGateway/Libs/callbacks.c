/*
 * callbacks.c
 *
 *  Created on: Jul 18, 2025
 *      Author: rizz
 */

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "LoraReceiver.h"
#include "llcc68_hal.h"
#include "app_freertos.h"

#define IRQ_TX_DONE (1 << 0)
#define IRQ_RX_DONE (1 << 1)
#define IRQ_TIMEOUT (1 << 8)

//extern uint16_t IRQ_FLAG;

//uint8_t rx_byte = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	HAL_GPIO_TogglePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin);

//	if (GPIO_Pin == LORA_DIO1_Pin) {

//		uint16_t irqFlags = LoRa_ReadIrqStatus();
//		IRQ_FLAG=irqFlags;
//		LoRa_ClearAllIrqStatus();
//
//		if (irqFlags & IRQ_TX_DONE) {
//			// передача завершена
//
//		} else if (irqFlags & IRQ_RX_DONE) {
//			// приём завершён
//			HAL_GPIO_TogglePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin);
//		} else if (irqFlags & IRQ_TIMEOUT) {
//			// таймаут
//		}
//	}
}

// коллбек HAL после приёма 1 байта
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

//	if (huart->Instance == USART1) {
//
//		// кладём байт в очередь
//		osMessageQueuePut(esp32_uart_queueHandle, huart->pRxBuffPtr, 0, 0);
//
//		// перезапускаем приём
//		HAL_UART_Receive_IT(huart, huart->pRxBuffPtr, 1);
//	}
}
