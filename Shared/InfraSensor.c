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
#include "BatteryLevel.h"
#include "main.h"

extern ADC_HandleTypeDef hadc2;

float get_infra_sensor_data(Battery_t Battery) {

	//Sensor stabilization for 100ms
//	HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
//	HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);
//	HAL_Delay(10);

// Polling sensor 5 times
	uint32_t adc_V0 = 0;

	for (int i = 0; i <= 3; i++) {

		HAL_ADC_Start(&hadc2);

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);

		TIM2_Delay_us(280);

		if (HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK) {
			uint32_t v_temp = HAL_ADC_GetValue(&hadc2);
			if (v_temp > adc_V0)
				adc_V0 = v_temp;
		}

		TIM2_Delay_us(40);

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);

		HAL_ADC_Stop(&hadc2);

		TIM2_Delay_us(10000);
	}

	/*
	 *  Vmax(100%)=5V
	 *  R1=220
	 *  R2=330
	 *  k=0.6
	 *  => Vmax = 3V
	 */

	float sensor_data = (5.0f / 3.0f) * adc_V0 * Battery.vdd / 4096.0f;

	return sensor_data;
}
