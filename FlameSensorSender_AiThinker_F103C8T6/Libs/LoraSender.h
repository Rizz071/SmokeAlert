/*
 * LoraSender.h
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#ifndef INC_LORASENDER_H_
#define INC_LORASENDER_H_

#include "Types.h"

void send_packet(SPI_HandleTypeDef *hspi, SendPacket_t packet);

#endif /* INC_LORASENDER_H_ */
