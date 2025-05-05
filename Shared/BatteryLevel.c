/*
 * BatteryLevel.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "BatteryLevel.h"
#include "stm32f1xx_hal.h"
#include <math.h>
#include <stdint.h>
#include "Types.h"

extern ADC_HandleTypeDef hadc1;

extern Battery_t Battery;

static uint8_t battery_percent_from_voltage(float);

Battery_t get_battery_level() {

	HAL_ADC_Start(&hadc1);

	uint32_t adc_val;
	if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
		adc_val = HAL_ADC_GetValue(&hadc1);
	}

	HAL_ADC_Stop(&hadc1);

	float vref = 1.20f;
	Battery.vdd = vref * 4096.0f / adc_val;
	Battery.charge_percent = battery_percent_from_voltage(Battery.vdd);

	return Battery;
}

static uint8_t battery_percent_from_voltage(float voltage) {

	//Approximation for cr123a BAT
	float q = 100.0f / (1.0f + expf(-20.0f * (voltage - 2.85f)));

	if (q < 0.0f)
		q = 0.0f;

	if (q > 100.0f)
		q = 100.0f;

	return (uint8_t) (q + 0.5f);
}
