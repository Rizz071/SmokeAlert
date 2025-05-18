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
#include "DebugLog.h"

extern ADC_HandleTypeDef hadc1;

extern Battery_t Battery;

static uint8_t battery_percent_from_voltage(float);

Battery_t get_battery_level() {

	debug("Polling for battery status...");
	HAL_ADC_Start(&hadc1);

	uint16_t adc_val = 0;
	uint16_t temp_val;

	for (uint8_t i = 0; i <= 2; i++) {

		HAL_ADC_PollForConversion(&hadc1, 10);

		temp_val = (uint16_t) HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Stop(&hadc1);

		if (temp_val > adc_val)
			adc_val = temp_val;

	}

	/*
	 *  Vmax(100%)=4.5V
	 *  R1=2.2k
	 *  R2=3.3K
	 *  k=1.675
	 *  => Vmax = 2.7V
	 */

	float vref = 3.3f;

	Battery.vdd = (vref / 2.7f) * vref * adc_val / 4095.0f;

	Battery.charge_percent = battery_percent_from_voltage(Battery.vdd);

	debug("Battery status:");
	debug("\tVDD: %d.%02d", (uint8_t) Battery.vdd,
			(uint16_t) (Battery.vdd * 100) % 100);
	debug("\tPercent: %d\n\r", Battery.charge_percent);

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
