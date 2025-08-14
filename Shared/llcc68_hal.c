/*
 * llcc68_hal.c
 *
 *  Created on: Jul 16, 2025
 *      Author: rizz
 */

#include "main.h"
#include "llcc68_hal.h"
#include <string.h>
#include "DebugLog.h"

#define NSS_LOW() HAL_GPIO_WritePin(LLCC68_PORT_NSS, LLCC68_Pin_NSS, GPIO_PIN_RESET)
#define NSS_HIGH() HAL_GPIO_WritePin(LLCC68_PORT_NSS, LLCC68_Pin_NSS, GPIO_PIN_SET)
#define RESET_LOW() HAL_GPIO_WritePin(LLCC68_PORT_NRST, LLCC68_Pin_NRST, GPIO_PIN_RESET)
#define RESET_HIGH() HAL_GPIO_WritePin(LLCC68_PORT_NRST, LLCC68_Pin_NRST, GPIO_PIN_SET)
#define BUSY_READ() HAL_GPIO_ReadPin(LLCC68_PORT_BUSY, LLCC68_Pin_BUSY)

static SPI_HandleTypeDef *hspi;

static GPIO_TypeDef *LLCC68_PORT_NRST;
static GPIO_TypeDef *LLCC68_PORT_NSS;
static GPIO_TypeDef *LLCC68_PORT_BUSY;

static uint16_t LLCC68_Pin_NRST;
static uint16_t LLCC68_Pin_NSS;
static uint16_t LLCC68_Pin_BUSY;

void LLCC68_Spi_Driver_Init(SPI_HandleTypeDef *hspi_extern,
		GPIO_TypeDef *GPIO_PORT_NRST, uint16_t GPIO_Pin_NRST,
		GPIO_TypeDef *GPIO_PORT_NSS, uint16_t GPIO_Pin_NSS,
		GPIO_TypeDef *GPIO_PORT_BUSY, uint16_t GPIO_Pin_BUSY) {

	hspi = hspi_extern;

	LLCC68_PORT_NRST = GPIO_PORT_NRST;
	LLCC68_PORT_NSS = GPIO_PORT_NSS;
	LLCC68_PORT_BUSY = GPIO_PORT_BUSY;

	LLCC68_Pin_NRST = GPIO_Pin_NRST;
	LLCC68_Pin_NSS = GPIO_Pin_NSS;
	LLCC68_Pin_BUSY = GPIO_Pin_BUSY;
}

void LLCC68_SetModulationParams() {

	// Настройка параметров модуляции LoRa
	uint8_t mod_params[4] = { 0x09, 0x04, 0x04, 0x01 }; // SF9, BW=125 кГц, CR=4/8, Low Data Rate enable
	LLCC68_WriteCommand(0x8B, mod_params, 4);

}

void LLCC68_SetPacketParams(uint8_t payload_length) {

	uint8_t packet_params[9];

	packet_params[0] = 0x00; // Preamble length MSB: number of symbols sent as preamble
	packet_params[1] = 0x10;	// Preamble length LSB (16)

	packet_params[2] = 0x00;	// Позиция заголовка (0x00 для явного заголовка)
								// When the byte headerType is at 0x00, the payload length, coding rate
								// and the header CRC will be added to the LoRa®
								// header and transported to the receiver.

	packet_params[3] = payload_length;	// Длина полезной нагрузки
										// Size of the payload (in bytes) to transmit or maximum size of the
										// payload that the receiver can accept.

	packet_params[4] = 0x01;			// CRC включён/выключен
	packet_params[5] = 0x00;			// Стандартный IQ
	packet_params[6] = 0x00;
	packet_params[7] = 0x00;
	packet_params[8] = 0x00;

	LLCC68_WriteCommand(0x8C, packet_params, 9);
}

void LLCC68_WaitWhileBusy(void) {
	while (BUSY_READ() == GPIO_PIN_SET)
		;
}

void LLCC68_Reset(void) {
	RESET_LOW();
	HAL_Delay(10);
	RESET_HIGH();
	HAL_Delay(20);
}

// helper: convert MHz to frequency word (32-bit)
void LLCC68_SetRfFrequency(uint32_t freq_hz) {
	uint32_t frf = (uint32_t) ((double) (freq_hz) * 1.048576); // (32 MHz / 2^25)
	uint8_t buf[4];
	buf[0] = (frf >> 24) & 0xFF;
	buf[1] = (frf >> 16) & 0xFF;
	buf[2] = (frf >> 8) & 0xFF;
	buf[3] = frf & 0xFF;

	LLCC68_WriteCommand(0x86, buf, 4);
}

// Отправка команды + чтение 2 байтов статуса
uint8_t LLCC68_CheckStatus(void) {
	uint8_t cmd = 0x80;
	uint8_t param = 0x00; // Standby RC
	uint8_t rx_buf[2] = { 0 };

	LLCC68_WaitWhileBusy();
	NSS_LOW();
	HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(hspi, &param, 1, HAL_MAX_DELAY);
	NSS_HIGH();
	HAL_Delay(1);

	// Теперь делаем "нулевую" передачу, чтобы вычитать статус (модуль возвращает его)
	LLCC68_WaitWhileBusy();
	NSS_LOW();
	uint8_t dummy[2] = { 0x00, 0x00 };
	HAL_SPI_TransmitReceive(hspi, dummy, rx_buf, 2, HAL_MAX_DELAY);
	NSS_HIGH();

	return rx_buf[0];  // Первый байт = статус
}

void LLCC68_WriteCommand(uint8_t cmd, uint8_t *data, uint8_t len) {
	LLCC68_WaitWhileBusy();
	NSS_LOW();
	HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(hspi, data, len, HAL_MAX_DELAY);
	NSS_HIGH();
	LLCC68_WaitWhileBusy();
}

void LLCC68_ReadCommand(uint8_t cmd, uint8_t *data, uint8_t len) {
	uint8_t dummy = 0x00;
	LLCC68_WaitWhileBusy();
	NSS_LOW();
	HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(hspi, &dummy, 1, HAL_MAX_DELAY); // dummy byte
	HAL_SPI_Receive(hspi, data, len, HAL_MAX_DELAY);
	NSS_HIGH();
	LLCC68_WaitWhileBusy();
}

/**
 * Чтение данных из буфера LLCC68 (команда ReadBuffer, 0x1E).
 * @param hspi Указатель на структуру SPI.
 * @param offset Смещение в буфере для начала чтения.
 * @param data Указатель на буфер для записи данных.
 * @param len Количество байт для чтения.
 * @return 0 при успехе, 1 при ошибке.
 */
uint8_t LLCC68_ReadBuffer(uint8_t offset, uint8_t *data, uint8_t len) {

	uint8_t tx_buf[3 + len];  // 3 байта команды + len NOP
	uint8_t rx_buf[3 + len];  // столько же, сколько tx

	tx_buf[0] = 0x1E;       // ReadBuffer opcode
	tx_buf[1] = offset;     // Offset в буфере
	tx_buf[2] = 0x00;

	memset(&tx_buf[3], 0x00, len); // Остальные NOP для получения данных

	LLCC68_WaitWhileBusy();
	NSS_LOW();
	HAL_SPI_TransmitReceive(hspi, tx_buf, rx_buf, 3 + len, HAL_MAX_DELAY);
	NSS_HIGH();

	memcpy(data, &rx_buf[3], len); // Пропускаем 3 служебных байта и читаем payload

	LLCC68_WaitWhileBusy();

	return 0;
}

uint8_t LLCC68_WriteBuffer(uint8_t offset, uint8_t *data, uint8_t len) {

	uint8_t tx_buf[2 + len];  // 2 байта команды + len NOP

	tx_buf[0] = 0x0E;       // ReadBuffer opcode
	tx_buf[1] = offset;     // Offset в буфере

	memcpy(&tx_buf[2], data, len); // Остальные NOP для получения данных

	LLCC68_WaitWhileBusy();
	NSS_LOW();
	HAL_SPI_Transmit(hspi, tx_buf, 2 + len, HAL_MAX_DELAY);
	NSS_HIGH();

	LLCC68_WaitWhileBusy();

	return 0;
}

uint16_t LLCC68_ReadIrqStatus() {

	uint8_t tx_buf[3] = { 0x12, 0x00, 0x00 };  // Opcode + NOPs
	uint8_t rx_buf[4] = { 0 };  // RFU + Status + IrqStatus_H + IrqStatus_L

	LLCC68_WaitWhileBusy();

	NSS_LOW();
	HAL_SPI_Transmit(hspi, tx_buf, sizeof(tx_buf), HAL_MAX_DELAY);
	HAL_SPI_Receive(hspi, rx_buf, sizeof(rx_buf), HAL_MAX_DELAY);
	NSS_HIGH();

	LLCC68_WaitWhileBusy();

	uint16_t irq = (rx_buf[2] << 8) | rx_buf[3];  // IrqStatus
	return irq;
}

void LLCC68_ClearAllIrqStatus() {
	uint8_t tx[] = { 0x02, 0xFF, 0xFF }; // Сбросить все биты IrqStatus

	LLCC68_WaitWhileBusy();

	NSS_LOW();
	HAL_SPI_Transmit(hspi, tx, sizeof(tx), HAL_MAX_DELAY);
	NSS_HIGH();

	LLCC68_WaitWhileBusy();
}
