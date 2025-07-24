/*
 * LoraSender.h
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#ifndef INC_LORARECEIVER_H_
#define INC_LORARECEIVER_H_

#include "Types.h"

uint8_t check_for_packet(uint8_t* buffer, uint8_t maxlen);
void LoRa_RX_Continuous_Init(SPI_HandleTypeDef *lora_hspi);

#endif /* INC_LORARECEIVER_H__ */
