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

extern SPI_HandleTypeDef hspi1;

static LoRa myLoRa;

static void init_LoRa() {
	myLoRa = newLoRa();

	myLoRa.CS_port = NSS_SIGNAL_GPIO_Port;
	myLoRa.CS_pin = NSS_SIGNAL_Pin;
	myLoRa.reset_port = RST_SIGNAL_GPIO_Port;
	myLoRa.reset_pin = RST_SIGNAL_Pin;
	myLoRa.DIO0_port = DIO0_INT_GPIO_Port;
	myLoRa.DIO0_pin = DIO0_INT_Pin;
	myLoRa.hSPIx = &hspi1;

	myLoRa.frequency = 433;             // default = 433 MHz
//	myLoRa.spredingFactor = SF_12;            // default = SF_7
	myLoRa.bandWidth = BW_125KHz;       // default = BW_125KHz
	myLoRa.crcRate = CR_4_8;          // default = CR_4_5
	myLoRa.power = POWER_17db;      // default = 20db
	myLoRa.overCurrentProtection = 130;             // default = 100 mA
	myLoRa.preamble = 12;              // default = 8;

	while (LoRa_init(&myLoRa) != 200)
		;
}

void send_packet(SendPacket_t packet) {
	init_LoRa();

	LoRa_transmit(&myLoRa, (uint8_t*) &packet, sizeof(SendPacket_t), 100);
}
