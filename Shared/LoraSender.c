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

//
#define RegOpMode 0x01 // (RegOpMode): Режим работы (LoRa, Sleep/Standby/TX).
//#define RegFrfMsb 0x0D // (RegFrfMsb), 0x0E (RegFrfMid), 0x0F (RegFrfLsb): Частота (433 МГц).
#define RegPaConfig 0x09 // (RegPaConfig): Мощность TX (+17 дБм, PA_BOOST).
#define RegModemConfig1 0x1D // (RegModemConfig1): BW (125 кГц), CR (4/8), заголовок.
#define RegModemConfig2 0x1E // (RegModemConfig2): SF (12), CRC.
#define RegModemConfig3 0x26 // (RegModemConfig3): Low Data Rate Optimization (не требуется для BW = 125 кГц).
#define RegOcp 0x0B // Current protection
#define RegFifoTxBaseAddr 0x0E
#define RegFifoRxBaseAddr 0x0F
#define RegFifoAddrPtr 0x0D

#define RegSyncWord 0x39
//#define RegPreambleMsb 0x0C // (RegPreambleMsb), 0x0D (RegPreambleLsb): Длина преамбулы (8).
//#define RegPayloadLength 0x22 // (RegPayloadLength): Длина полезной нагрузки (5 байт).
//#define RegFifo 0x40 // (RegFifo): Буфер FIFO для данных.
//#define RegIrqFlags 0x39 // (RegIrqFlags): Флаги прерываний (например, TxDone).
//#define RegDioMapping1 0x3D // (RegDioMapping1): Настройка DIO0 (TxDone).

extern SPI_HandleTypeDef hspi1;

LoRa myLoRa;

void SX1278_Init(void) {
	myLoRa = newLoRa();

	myLoRa.CS_port = NSS_SIGNAL_GPIO_Port;
	myLoRa.CS_pin = NSS_SIGNAL_Pin;
	myLoRa.reset_port = GPIOB;
	myLoRa.reset_pin = RST_SIGNAL_Pin;
	myLoRa.DIO0_port = DIO0_INT_GPIO_Port;
	myLoRa.DIO0_pin = DIO0_INT_Pin;
	myLoRa.hSPIx = &hspi1;

	LoRa_init(&myLoRa);
}

void send_packet(SendPacket_t packet) {

	SX1278_Init();

	LoRa_transmit(&myLoRa, (uint8_t*) &packet, sizeof(SendPacket_t), 100);
//	SX1278_Transmit((uint8_t*) &packet, sizeof(SendPacket_t));
}
