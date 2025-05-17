/*
 * InfraSensor.h
 *
 *  Created on: May 5, 2025
 *      Author: rizz
 */

#ifndef INFRASENSOR_H_
#define INFRASENSOR_H_

#include "main.h"

float get_infra_sensor_data(ADC_HandleTypeDef *ha, float alarm_lvl);

#endif /* INFRASENSOR_H_ */
