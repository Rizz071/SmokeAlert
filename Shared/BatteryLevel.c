/*
 * BatteryLevel.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "BatteryLevel.h"
#include "main.h"
#include <math.h>
#include <stdint.h>
#include "Types.h"
#include "DebugLog.h"

extern Battery_t Battery;

static uint8_t battery_percent_from_voltage(float);

Battery_t get_battery_level(ADC_HandleTypeDef *hadc) {

	debug("Polling for battery status...");

	uint16_t adc_val = 0;
	uint16_t temp_val;

	HAL_ADCEx_Calibration_Start(hadc);

	for (uint8_t i = 1; i <= 3; i++) {

		HAL_ADC_Start(hadc);

		HAL_ADC_PollForConversion(hadc, 10);

		temp_val = (uint16_t) HAL_ADC_GetValue(hadc);

		HAL_ADC_Stop(hadc);

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
