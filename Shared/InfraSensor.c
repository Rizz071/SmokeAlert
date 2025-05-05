/*
 * InfraSensor.c
 *
 *  Created on: May 5, 2025
 *      Author: rizz
 */

#include "InfraSensor.h"
#include "stm32f1xx_hal.h"
#include "Delay_ns.h"
#include "Types.h"
#include "main.h"

extern ADC_HandleTypeDef hadc2;
extern Battery_t Battery;

float get_infra_sensor_data() {

	//Sensor stabilization for 100ms
	HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	// Polling sensor 5 times
	uint32_t adc_V0 = 0;

	for (int i = 0; i <= 4; i++) {

		HAL_ADC_Start(&hadc2);

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);

		TIM2_Delay_us(28);

		if (HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK) {
			uint32_t v_temp = HAL_ADC_GetValue(&hadc2);
			if (v_temp > adc_V0)
				adc_V0 = v_temp;
		}

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);

		HAL_ADC_Stop(&hadc2);

		HAL_Delay(11);
	}

	float Vin = adc_V0 * (Battery.vdd / 4096.0f);
	float sensor_data = Vin * 1.67f;

	return sensor_data;
}
