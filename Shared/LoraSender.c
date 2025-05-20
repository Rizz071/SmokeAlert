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
#include "DebugLog.h"

static LoRa myLoRa;

void SX1278_Init(SPI_HandleTypeDef *hspi) {

	myLoRa = newLoRa();

	myLoRa.CS_port = NSS_SIGNAL_GPIO_Port;
	myLoRa.CS_pin = NSS_SIGNAL_Pin;
	myLoRa.reset_port = GPIOB;
	myLoRa.reset_pin = RST_SIGNAL_Pin;
	myLoRa.DIO0_port = DIO0_INT_GPIO_Port;
	myLoRa.DIO0_pin = DIO0_INT_Pin;
	myLoRa.hSPIx = hspi;

	LoRa_init(&myLoRa);
}

void send_packet(SPI_HandleTypeDef *hspi, SendPacket_t packet) {

	debug("Enabling LoRa...");
	SX1278_Init(hspi);

	uint8_t max_tries_to_send = 3;
	uint8_t current_try = 0;

	debug("Sending packet...");

	while (1) {

		current_try++;

		if (LoRa_transmit(&myLoRa, (uint8_t*) &packet, sizeof(SendPacket_t),
				100) == 1) {
			debug("\t...DONE at %d try!\n\r", current_try);
			break;
		}

		if (current_try >= max_tries_to_send) {
			debug("...FAILED. Skipping!\n\r");
			break;
		}

		HAL_Delay(100);

	}
}
