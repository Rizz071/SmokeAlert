#include "main.h"
#include "LoraReceiver.h"
#include "llcc68_hal.h"
#include "DebugLog.h"
#include <string.h>

// Определения флагов IRQ для LLCC68 (согласно datasheet)
#define IRQ_RX_DONE         (1 << 1)  // 0x0002: Пакет принят
#define IRQ_RX_TX_TIMEOUT   (1 << 6)  // 0x0040: Тайм-аут приёма/передачи
#define IRQ_CRC_ERR         (1 << 9)  // 0x0200: Ошибка CRC
#define IRQ_HEADER_ERR      (1 << 10) // 0x0400: Ошибка заголовка
#define IRQ_SYNCWORD_ERR    (1 << 11) // 0x0800: Ошибка синхрослова

// Глобальная переменная для хранения IRQ (если требуется для других модулей)
volatile uint16_t IRQ_FLAG = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//
//	HAL_GPIO_TogglePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin);
//
//	if (GPIO_Pin == LORA_DIO1_Pin) {
//		// Переключение светодиода для индикации
//		HAL_GPIO_TogglePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin);
//
//		// Чтение статуса IRQ
//		uint16_t irqFlags = LoRa_ReadIrqStatus();
//		IRQ_FLAG = irqFlags; // Сохранение для возможного использования
//		debug("Receiver IRQ: 0x%04X\n\r", irqFlags);
//
//		// Очистка всех флагов IRQ
//		LoRa_ClearAllIrqStatus();
//
//		// Обработка RX_DONE (успешный приём)
//		if (irqFlags & IRQ_RX_DONE) {
//			debug("Packet received!\n\r");
//
//			// Чтение длины пакета и RSSI
//			uint8_t rx_status[2] = { 0 };
//			LLCC68_ReadCommand(0x13, rx_status, 2); // GetRxBufferStatus
//			uint8_t rx_len = rx_status[0]; // Длина полезной нагрузки
//			uint8_t rssi = rx_status[1];   // RSSI
//
//			// Проверка корректности длины
//			if (rx_len > 0 && rx_len <= 255) {
//				// Чтение пакета
//				uint8_t buffer[rx_len + 1];
//				buffer[0] = 0x00; // RX base address
//				LLCC68_ReadCommand(0x1D, buffer, rx_len + 1); // ReadBuffer
//				buffer[rx_len + 1] = '\0'; // Null-терминатор для строки
//
//				// Вывод принятых данных и RSSI
//				debug("Received: %s\n\r", &buffer[1]);
//				debug("RSSI: %d dBm\n\r", rssi);
//			} else {
//				debug("Invalid packet length: %d\n\r", rx_len);
//			}
//
//			// Перезапуск RX
//			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF }; // Continuous RX
//			LLCC68_WriteCommand(0x82, rx_timeout, 3);
//		}
//		// Обработка ошибок
//		else if (irqFlags & IRQ_RX_TX_TIMEOUT) {
//			debug("Receive timeout!\n\r");
//			// Перезапуск RX
//			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
//			LLCC68_WriteCommand(0x82, rx_timeout, 3);
//		} else if (irqFlags & IRQ_CRC_ERR) {
//			debug("CRC error!\n\r");
//			// Перезапуск RX
//			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
//			LLCC68_WriteCommand(0x82, rx_timeout, 3);
//		} else if (irqFlags & IRQ_HEADER_ERR) {
//			debug("Header error!\n\r");
//			// Перезапуск RX
//			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
//			LLCC68_WriteCommand(0x82, rx_timeout, 3);
//		} else if (irqFlags & IRQ_SYNCWORD_ERR) {
//			debug("Sync word error!\n\r");
//			// Перезапуск RX
//			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
//			LLCC68_WriteCommand(0x82, rx_timeout, 3);
//		} else {
//			debug("Unknown IRQ flags: 0x%04X\n\r", irqFlags);
//			// Перезапуск RX для безопасности
//			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
//			LLCC68_WriteCommand(0x82, rx_timeout, 3);
//		}
//
//		// Проверка статуса модуля после обработки
//		uint8_t status[1];
//		LLCC68_ReadCommand(0xC0, status, 1);
//		debug("Module status after IRQ: 0x%02X\n\r", status[0]);
//	}
}
