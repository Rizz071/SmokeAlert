/*
 * BatteryLevel.h
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#ifndef INC_BATTERYLEVEL_H_
#define INC_BATTERYLEVEL_H_

#include "main.h"
#include "Types.h"

Battery_t get_battery_level(ADC_HandleTypeDef *hadc);

#endif /* INC_BATTERYLEVEL_H_ */
