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

	/*
	 *  Vmax(100%)=4.5V
	 *  R1=2.2k
	 *  R2=3.3K
	 *  k=1.675
	 *  => Vmax = 2.7V
	 */

	float vref = 3.3f;

	Battery.vdd = (vref / 2.7f) * vref * adc_val / 4096.0f;

	Battery.charge_percent = battery_percent_from_voltage(Battery.vdd);

	return Battery;
}

static uint8_t battery_percent_from_voltage(float voltage) {

	if (voltage >= 3.2f)
		return 100;
	else if (voltage >= 2.9f)
		return (uint8_t) (80 + (voltage - 2.9f) / 0.3f * 20);
	else if (voltage >= 2.5f)
		return (uint8_t) (20 + (voltage - 2.5f) / 0.4f * 60);
	else if (voltage >= 2.0f)
		return (uint8_t) ((voltage - 2.0f) / 0.5f * 20);
	else
		return 0;
}
