/*
 * LoraSender.h
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#ifndef INC_LORARECEIVER_H_
#define INC_LORARECEIVER_H_

#include "Types.h"

void check_for_packet();
void LoRa_RX_Continuous_Init(SPI_HandleTypeDef *lora_hspi);

#endif /* INC_LORARECEIVER_H__ */
