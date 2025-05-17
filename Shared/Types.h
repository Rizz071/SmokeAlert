/*
 * Types.h
 *
 *  Created on: May 4, 2025
 *      Author: rizz
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef struct SerialNumber_t {
	uint32_t byte_0;
	uint32_t byte_1;
	uint32_t byte_2;
} SerialNumber_t;

typedef struct SendPacket_t {
	uint32_t ID;
	float sensor_data;
	uint8_t battery_level;
} SendPacket_t;

typedef struct Battery_t {
	float vdd;
	uint8_t charge_percent;
} Battery_t;

#endif /* TYPES_H_ */
