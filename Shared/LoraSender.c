/*
 * LoraSender.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "stm32f1xx_hal.h"
#include "LoraSender.h"
#include "LoRa.h"
#include "Types.h"
#include <stdint.h>
#include "stdio.h"
//#include "main.h"

//
#define RegOpMode 0x01 // (RegOpMode): Режим работы (LoRa, Sleep/Standby/TX).
//#define RegFrfMsb 0x0D // (RegFrfMsb), 0x0E (RegFrfMid), 0x0F (RegFrfLsb): Частота (433 МГц).
#define RegPaConfig 0x09 // (RegPaConfig): Мощность TX (+17 дБм, PA_BOOST).
#define RegModemConfig1 0x1D // (RegModemConfig1): BW (125 кГц), CR (4/8), заголовок.
#define RegModemConfig2 0x1E // (RegModemConfig2): SF (12), CRC.
#define RegModemConfig3 0x26 // (RegModemConfig3): Low Data Rate Optimization (не требуется для BW = 125 кГц).
#define RegOcp 0x0B // Current protection
#define RegSyncWord 0x39
//#define RegPreambleMsb 0x0C // (RegPreambleMsb), 0x0D (RegPreambleLsb): Длина преамбулы (8).
//#define RegPayloadLength 0x22 // (RegPayloadLength): Длина полезной нагрузки (5 байт).
//#define RegFifo 0x40 // (RegFifo): Буфер FIFO для данных.
//#define RegIrqFlags 0x39 // (RegIrqFlags): Флаги прерываний (например, TxDone).
//#define RegDioMapping1 0x3D // (RegDioMapping1): Настройка DIO0 (TxDone).

extern SPI_HandleTypeDef hspi1;

LoRa myLoRa;


void SX1278_WriteReg(uint8_t addr, uint8_t value) {
	HAL_GPIO_WritePin(NSS_SIGNAL_GPIO_Port, NSS_SIGNAL_Pin, GPIO_PIN_RESET);
	uint8_t buf[2] = { addr | 0x80, value };
	HAL_SPI_Transmit(&hspi1, buf, 2, 100);
	HAL_GPIO_WritePin(NSS_SIGNAL_GPIO_Port, NSS_SIGNAL_Pin, GPIO_PIN_SET);
}

uint8_t SX1278_ReadReg(uint8_t addr) {
	uint8_t tx[2] = { addr & 0x7F, 0x00 };
	uint8_t rx[2] = { 0 };
	HAL_GPIO_WritePin(NSS_SIGNAL_GPIO_Port, NSS_SIGNAL_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 100);
	HAL_GPIO_WritePin(NSS_SIGNAL_GPIO_Port, NSS_SIGNAL_Pin, GPIO_PIN_SET);
	return rx[1];
}

void SX1278_Init(void) {

	myLoRa = newLoRa();

	myLoRa.CS_port = NSS_SIGNAL_GPIO_Port;
	myLoRa.CS_pin = NSS_SIGNAL_Pin;
	myLoRa.reset_port = RST_SIGNAL_GPIO_Port;
	myLoRa.reset_pin = RST_SIGNAL_Pin;
	myLoRa.DIO0_port = DIO0_INT_GPIO_Port;
	myLoRa.DIO0_pin = DIO0_INT_Pin;
	myLoRa.hSPIx = &hspi1;

	myLoRa.frequency             = 433;             // default = 433 MHz
	myLoRa.spredingFactor        = SF_7;            // default = SF_7
	myLoRa.bandWidth             = BW_125KHz;       // default = BW_125KHz
	myLoRa.crcRate               = CR_4_8;          // default = CR_4_5
	myLoRa.power                 = POWER_17db;      // default = 20db
	myLoRa.overCurrentProtection = 130;             // default = 100 mA
	myLoRa.preamble              = 8;              // default = 8;

	HAL_Delay(10);

	// Sleep mode, LoRa mode
	SX1278_WriteReg(RegOpMode, 0x80); // RegOpMode = LoRa + Sleep
	HAL_Delay(10);

	// Set frequency 433 MHz
	SX1278_WriteReg(0x06, 0x6C); // RegFrMsb
	SX1278_WriteReg(0x07, 0x80); // RegFrMid
	SX1278_WriteReg(0x08, 0x00); // RegFrLsb

	// FIFO TX base addr
	SX1278_WriteReg(0x0E, 0x00); // RegFifoTxBaseAddr
	SX1278_WriteReg(0x0D, 0x00); // RegFifoAddrPtr

	// Max power
	SX1278_WriteReg(RegPaConfig, 0b10001111); // RegPaConfig

	SX1278_WriteReg(RegOcp, 0b00110000); // 130mA

	SX1278_WriteReg(0x0C, 0x23); // RegLna: LnaGain=G1, LnaBoostHf=11
//
//	// RegModemConfig1 = BW=7.8kHz, CR=4/5
	SX1278_WriteReg(RegModemConfig1, 0b01110010);
//
//	// RegModemConfig2 = SF=12, CRC on
	SX1278_WriteReg(RegModemConfig2, 0b01110100);
////
//	RegModemConfig3 = ImplicitHeaderModeOn
	SX1278_WriteReg(RegModemConfig3, 0b00000100);

	SX1278_WriteReg(RegSyncWord, 0x12); // Sync Word

}

void SX1278_Transmit(uint8_t *data, uint8_t len) {

	// Set standby
	SX1278_WriteReg(0x01, 0x81); // RegOpMode = LoRa + Standby

	// Set FIFO pointer
	SX1278_WriteReg(0x0D, 0x00);

	// Write data to FIFO
	for (uint8_t i = 0; i < len; i++) {
		SX1278_WriteReg(0x00, data[i]);
	}

	// Set payload length
	SX1278_WriteReg(0x22, len);

	// Set TX mode
	SX1278_WriteReg(RegOpMode, 0x83); // RegOpMode = LoRa + TX

	// Wait for TxDone
	while ((SX1278_ReadReg(0x12) & 0x08) == 0)
		;

	// Clear IRQ flags
	SX1278_WriteReg(0x12, 0xFF);
}

void send_packet(SendPacket_t packet) {

	SX1278_Init();

//	LoRa_transmit(&myLoRa, (uint8_t*) &packet, sizeof(SendPacket_t), 100);
	SX1278_Transmit((uint8_t*) &packet, sizeof(SendPacket_t));
}
