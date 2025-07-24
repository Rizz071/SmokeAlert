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

//static SPI_HandleTypeDef *hspi;

void LoRa_RX_Continuous_Init(SPI_HandleTypeDef *lora_hspi) {

//	SPI_HandleTypeDef *hspi = lora_hspi;

//	debug("Lora init started...\r\n");

//	LLCC68_Spi_Driver_Init(lora_hspi, GPIOA, LORA_NRST_Pin, GPIOA,
//	LORA_NSS_Pin, LORA_BUSY_GPIO_Port, LORA_BUSY_Pin);
//
//	LLCC68_Reset();
//
//	// Set Standby mode
//	uint8_t standby_param = 0x00; // RC
//	LLCC68_WriteCommand(0x80, &standby_param, 1);
//
//	// Set Packet Type to LoRa
//	uint8_t packet_type = 0x01;
//	LLCC68_WriteCommand(0x8A, &packet_type, 1);

	// Set RF frequency
//	uint32_t freq = 910163968; // 868 МГц в формате PLL
//	uint8_t buf[4];
//	buf[0] = (uint8_t) (freq >> 24);
//	buf[1] = (uint8_t) (freq >> 16);
//	buf[2] = (uint8_t) (freq >> 8);
//	buf[3] = (uint8_t) (freq);
//	LLCC68_WriteCommand(0x86, buf, 4); // 0x86 — SetRfFrequency

	// Set buffer base address
//	uint8_t buf_addr[2] = { 0x0, 0x80 };
//	LLCC68_WriteCommand(0x8F, buf_addr, 2);

//	// SetModulationParams(uint8_t spreadingFactor, uint8_t bandwidth, uint8_t codingRate, uint8_t lowDataRateOptimize)
//    uint8_t params1[4];
//    params1[0] = 7;    // Spreading Factor SF7 (7..12)
//    params1[1] = 9;    // Bandwidth 125 kHz (примерное значение для 125kHz)
//    params1[2] = 1;    // Coding Rate 4/5
//    params1[3] = 0;    // Low Data Rate Optimize off (0)
//    LLCC68_WriteCommand(0x8B, params1, 4);

//    // SetPacketParams(uint8_t preambleLength[2], uint8_t headerType, uint8_t payloadLength, uint8_t crcMode, uint8_t invertIQ)
//    uint8_t params2[6];
//    params2[0] = 0x00; // preambleLength MSB (например, 8 байт прелюда)
//    params2[1] = 0x08; // preambleLength LSB (8 байт)
//    params2[2] = 0x00; // headerType (0 = variable length, 1 = fixed)
//    params2[3] = 32;   // payloadLength (максимальный размер пакета)
//    params2[4] = 1;    // CRC on (1)
//    params2[5] = 0;    // invert IQ off (0)
//    LLCC68_WriteCommand(0x8C, params2, 6);

	// Set all interrupts to DIO1
//	uint8_t params3[8];
//	uint16_t irqMask = 0x00FF;    // включаем IRQ биты 0-7
//	uint16_t dio1Mask = 0x00FF;   // все эти IRQ выводим на DIO1
//	uint16_t dio2Mask = 0x0000;   // DIO2 ничего не выводит
//	uint16_t dio3Mask = 0x0000;   // DIO3 ничего не выводит
//	params3[0] = (irqMask >> 8) & 0xFF;
//	params3[1] = irqMask & 0xFF;
//	params3[2] = (dio1Mask >> 8) & 0xFF;
//	params3[3] = dio1Mask & 0xFF;
//	params3[4] = (dio2Mask >> 8) & 0xFF;
//	params3[5] = dio2Mask & 0xFF;
//	params3[6] = (dio3Mask >> 8) & 0xFF;
//	params3[7] = dio3Mask & 0xFF;
//	LLCC68_WriteCommand(0x08, params3, 8);

//	// SetPaConfig(uint8_t paSelect, uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut)
//	uint8_t params[5] = { 0 };
//	params[0] = 0x01; // paSelect: 0x00 = RFO, 0x01 = PA_BOOST (для E220 это обычно 0x01)
//	params[1] = 0x01;  // paDutyCycle (для E220 не критично, можно 0x01)
//	params[2] = 0x07;  // hpMax (максимальная мощность, 0..7)
//	params[3] = 0x00;  // deviceSel (0 для E220)
//	params[4] = 0x01;  // paLut (таблица PA)
//	LLCC68_WriteCommand(0x95, params, 5);

	// Set TX power
//	uint8_t power[2] = { 0x11, 0x07 }; // Pwr=17dBm, ramp time
//	LLCC68_WriteCommand(0x8E, power, 2);

//
	// Start RX Cont
//	uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
	uint8_t rx_timeout[3] = { 0x00, 0x00, 0x00 };
	LLCC68_WriteCommand(0x82, rx_timeout, 3);

//	debug("Lora init finished...\r\n");

}

void check_for_packet() {

//	uint8_t rx_timeout[3] = { 0x00, 0x00, 0x00 };
//	LLCC68_WriteCommand(0x82, rx_timeout, 3);

//	uint8_t tx_data[] = "Test message";  // Буфер с передаваемыми данными
//
//	// Write payload to buffer
//	uint8_t tx_base_addr = 0x00;
//	LLCC68_WriteCommand(0x0E, &tx_base_addr, 1); // set offset
//	LLCC68_WriteCommand(0x0E, tx_data, sizeof(tx_data)); // write payload
//
//	uint8_t tx_timeout[3] = { 0x00, 0x00, 0x00 };
//	LLCC68_WriteCommand(0x83, tx_timeout, 3);

//	debug("Enabling LoRa...");
//
////	LoRa_RX_Continuous_Init(lora_hspi);
//
//	uint8_t tx_data[] = "Test message";  // Буфер с передаваемыми данными
//
//	// Write payload to buffer
//	uint8_t tx_base_addr = 0x00;
//	LLCC68_WriteCommand(0x0E, &tx_base_addr, 1);  // set offset
//	LLCC68_WriteCommand(0x0E, tx_data, sizeof(tx_data));  // write payload
//
//	// Set payload length
//	uint8_t tx_len = sizeof(tx_data);
//	LLCC68_WriteCommand(0x94, &tx_len, 1);

//	debug("Sending packet...");

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
}

