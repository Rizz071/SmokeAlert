/*
 * LoraSender.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "main.h"
#include "LoraSender.h"
#include "Types.h"
#include "DebugLog.h"
#include <stdio.h>
#include "llcc68_hal.h"
#include <string.h>

void send_packet(SPI_HandleTypeDef *hspi, SendPacket_t packet) {

	debug("Enabling LoRa...\n\r");
	HAL_Delay(100);

	LLCC68_Spi_Driver_Init(hspi, LORA_NRST_GPIO_Port, LORA_NRST_Pin,
	LORA_NSS_GPIO_Port, LORA_NSS_Pin, LORA_BUSY_GPIO_Port, LORA_BUSY_Pin);

	LLCC68_Reset();

	uint8_t status = LLCC68_CheckStatus();

	if ((status & 0xF0) != 0x00) {
		// Модуль ответил статусом => жив

		HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
				GPIO_PIN_SET); // светодиод ВЫКЛ (Bluepill style)
	} else {

		// Статус = 0x00 => модуль не отвечает
		HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
				GPIO_PIN_RESET);   // светодиод ВКЛ
	}

	// Set Standby mode
	uint8_t standby_param = 0x00; // RC
	LLCC68_WriteCommand(0x80, &standby_param, 1);

	// Set Packet Type to LoRa
	uint8_t packet_type = 0x01;
	LLCC68_WriteCommand(0x8A, &packet_type, 1);

	// Set RF frequency
	LLCC68_SetRfFrequency(870000000);

	// Настройка параметров PA (усилителя мощности)
	uint8_t pa_config[4] = { 0x04, 0x07, 0x00, 0x01 }; // Оптимальные настройки для 22 дБм
	LLCC68_WriteCommand(0x95, pa_config, 4);

	// Настройка параметров передачи
	uint8_t tx_params[2] = { 14, 0x00 }; // 14 дБм, Ramp time
	LLCC68_WriteCommand(0x8E, tx_params, 2);

	// Настройка параметров модуляции LoRa
	// Параметры задаются в драйвере LLCC68 сразу для всех устройств
	LLCC68_SetModulationParams();

	// Настройка параметров пакета
	LLCC68_SetPacketParams(sizeof(SendPacket_t));

	// Set all interrupts to DIO1
	uint8_t params3[8];
	uint16_t irqMask = 0x00FF;    // включаем IRQ биты 0-7
	uint16_t dio1Mask = 0x00FF;   // все эти IRQ выводим на DIO1
	uint16_t dio2Mask = 0x0000;   // DIO2 ничего не выводит
	uint16_t dio3Mask = 0x0000;   // DIO3 ничего не выводит
	params3[0] = (irqMask >> 8) & 0xFF;
	params3[1] = irqMask & 0xFF;
	params3[2] = (dio1Mask >> 8) & 0xFF;
	params3[3] = dio1Mask & 0xFF;
	params3[4] = (dio2Mask >> 8) & 0xFF;
	params3[5] = dio2Mask & 0xFF;
	params3[6] = (dio3Mask >> 8) & 0xFF;
	params3[7] = dio3Mask & 0xFF;
	LLCC68_WriteCommand(0x08, params3, 8);

	// Set buffer base address
	uint8_t base_addr[2] = { 0x00, 0x00 };  // TX = 0x00, RX = 0x00
	LLCC68_WriteCommand(0x8F, base_addr, 2);

	uint8_t buffer[sizeof(SendPacket_t)];
	memcpy(buffer, &packet, sizeof(SendPacket_t));

	LLCC68_WriteBuffer(0x00, buffer, sizeof(SendPacket_t));

	uint8_t tx_timeout[3] = { 0x00, 0x00, 0x00 };
	LLCC68_WriteCommand(0x83, tx_timeout, 3);

	// Проверка статуса IRQ
//	uint8_t irq_status[2];
//	LLCC68_ReadCommand(0x12, irq_status, 2); // GetIrqStatus

	debug("Packet sent!\n\r");
	LLCC68_ClearAllIrqStatus(); // Сброс флага
}

