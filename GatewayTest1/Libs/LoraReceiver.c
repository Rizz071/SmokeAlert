/*
 * LoraSender.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "main.h"
#include "LoraReceiver.h"
#include "Types.h"
#include "DebugLog.h"
#include "llcc68_hal.h"

static SPI_HandleTypeDef *hspi;

void LoRa_RX_Continuous_Init(SPI_HandleTypeDef *lora_hspi) {

	hspi = lora_hspi;
//	debug("Lora init started...\r\n");

	LLCC68_Spi_Driver_Init(hspi, LORA_NRST_GPIO_Port, LORA_NRST_Pin,
	LORA_NSS_GPIO_Port,
	LORA_NSS_Pin);

	SX1278_WriteReg(0x01, 0x81); // RegOpMode: LoRa + Standby

	// Частота 433.0 МГц
	SX1278_WriteReg(0x06, 0x6C); // RegFrfMsb
	SX1278_WriteReg(0x07, 0x80); // RegFrfMid
	SX1278_WriteReg(0x08, 0x00); // RegFrfLsb

	SX1278_WriteReg(0x09, 0xFF); // Max power — неважно для приёмника

	// FIFO адреса
	SX1278_WriteReg(0x0D, 0x00); // RegFifoAddrPtr
	SX1278_WriteReg(0x0E, 0x00); // RegFifoTxBaseAddr
	SX1278_WriteReg(0x0F, 0x00); // RegFifoRxBaseAddr

	SX1278_WriteReg(0x1E, 0x74); // RegModemConfig2: SF7, CRC off

	SX1278_WriteReg(0x22, 0xFF); // Max payload length

	SX1278_WriteReg(0x01, 0x85); // RegOpMode: LoRa + RxContinuous

//	LLCC68_Reset();
//
//	// Set Standby mode
//	uint8_t standby_param = 0x00; // RC
//	LLCC68_WriteCommand(0x80, &standby_param, 1);
//
//	// Set Packet Type to LoRa
//	uint8_t packet_type = 0x01;
//	LLCC68_WriteCommand(0x8A, &packet_type, 1);

//	// Set RF frequency
//	uint32_t freq = 910163968; // 868 МГц в формате PLL
//	uint8_t buf[4];
//	buf[0] = (uint8_t) (freq >> 24);
//	buf[1] = (uint8_t) (freq >> 16);
//	buf[2] = (uint8_t) (freq >> 8);
//	buf[3] = (uint8_t) (freq);
//	LLCC68_WriteCommand(0x86, buf, 4); // 0x86 — SetRfFrequency

//	// Set Sync Word
//	uint8_t sync_word[2] = { 0x14, 0x24 }; // Частная сеть LoRa
//	LLCC68_WriteRegister(0x0740, sync_word, 2);

//	// Set PA Config (PA_BOOST для 22 дБм)
//	uint8_t pa_config[5] = { 0x01, 0x01, 0x07, 0x00, 0x01 };
//	LLCC68_WriteCommand(0x95, pa_config, 5);

	// Set TX power
//	uint8_t power[2] = { 0x16, 0x07 }; // 22 дБм
//	LLCC68_WriteCommand(0x8E, power, 2);

//	// Set buffer base address
//	uint8_t buf_addr[2] = { 0x80, 0x00 }; // TX base: 0x80, RX base: 0x00
//	LLCC68_WriteCommand(0x8F, buf_addr, 2);
//
//	// Set Modulation Params (SF7, BW 250 кГц, CR 4/5)
//	uint8_t mod_params[4] = { 0x07, 0x05, 0x01, 0x00 }; // SF7, BW 250 кГц, CR 4/5, LowDR off
//	LLCC68_WriteCommand(0x8B, mod_params, 4);
//
//	// Set Packet Params
//	uint8_t pkt_params[6] = { 0x00, 0x08, 0x00, 0xFF, 0x01, 0x00 }; // Preamble 8, variable length, max 255 bytes, CRC on
//	LLCC68_WriteCommand(0x8C, pkt_params, 6);
//
//	// Set IRQ to DIO1
//	uint8_t irq_params[8] = { 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00 };
//	LLCC68_WriteCommand(0x08, irq_params, 8);

}

uint8_t check_for_packet(uint8_t *buffer, uint8_t maxlen) {
	debug("Enabling LoRa Receiver...\n\r");

	if (SX1278_ReadReg(0x12) & 0x40) {  // RxDone

		HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
				GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
				GPIO_PIN_SET);

		uint8_t len = SX1278_ReadReg(0x13);     // RegRxNbBytes
		uint8_t addr = SX1278_ReadReg(0x10);    // RegFifoRxCurrentAddr
		SX1278_WriteReg(0x0D, addr);            // Указатель FIFO


		for (uint8_t i = 0; i < maxlen; i++) {
			buffer[i] = SX1278_ReadReg(0x00);   // RegFifo
		}

		SX1278_WriteReg(0x12, 0xFF); // Сброс флагов прерываний
		return len;

	}
	return 0;
//	// Start RX
//	uint8_t rx_timeout[3] = { 0x00, 0x00, 0x00 }; // Continuous RX
//	LLCC68_WriteCommand(0x82, rx_timeout, 3);
//
//	while (1) {
//		uint16_t irq_status = LoRa_ReadIrqStatus();
//		if (irq_status & 0x0002) { // RxDone
//			debug("Packet received!\n\r");
//
//			// Read packet length
//			uint8_t rx_len_cmd[2] = { 0x00, 0x00 };
//			LLCC68_ReadCommand(0x13, rx_len_cmd, 2); // GetRxBufferStatus
//			uint8_t rx_len = rx_len_cmd[0];
//
//			// Read packet
//			uint8_t buffer[rx_len + 1];
//			buffer[0] = 0x00; // RX base address
//			LLCC68_ReadCommand(0x1D, buffer, rx_len + 1); // ReadBuffer
//			buffer[rx_len + 1] = '\0'; // Null-terminate for string
//
//			debug("Received: %s\n\r", &buffer[1]);
//			debug("RSSI: %d dBm\n\r", rx_len_cmd[1]);
//
//			LoRa_ClearAllIrqStatus();
//			LLCC68_WriteCommand(0x82, rx_timeout, 3); // Restart RX
//		} else if (irq_status & 0x0004) { // RxTimeout or error
//			debug("Receive timeout or error!\n\r");
//			LoRa_ClearAllIrqStatus();
//			LLCC68_WriteCommand(0x82, rx_timeout, 3); // Restart RX
//		}
//		HAL_Delay(10);
}

//	while (1) {
//		HAL_Delay(1000);
//	}
//
//	uint8_t max_tries_to_send = 3;
//	uint8_t current_try = 0;
//
//	debug("Sending packet...");
//
//	while (1) {
//
//		current_try++;
//
//		if (LoRa_transmit(&myLoRa, (uint8_t*) &packet, sizeof(SendPacket_t),
//				100) == 1) {
//			debug("\t...DONE at %d try!\n\r", current_try);
//			break;
//		}
//
//		if (current_try >= max_tries_to_send) {
//			debug("...FAILED. Skipping!\n\r");
//			break;
//		}
//
//		HAL_Delay(100);
//
//	}

