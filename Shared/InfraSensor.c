/*
 * InfraSensor.c
 *
 *  Created on: May 5, 2025
 *      Author: rizz
 */

#include "InfraSensor.h"
#include "main.h"
#include "Delay_ns.h"
#include "DebugLog.h"

static float poll_sensor(ADC_HandleTypeDef *hadc, uint8_t times) {

//	HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);

	uint16_t adc_V0 = 0;

	for (uint8_t i = 1; i <= times; i++) {

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);

		HAL_ADCEx_Calibration_Start(hadc); // 3-5 us на stm32f103 при 8Mhz at ADC

		TIM2_Delay_us(280);

//		HAL_GPIO_WritePin(INFO_LED_GPIO_Port, INFO_LED_Pin, GPIO_PIN_SET);
//		HAL_Delay(1);
//		HAL_GPIO_WritePin(INFO_LED_GPIO_Port, INFO_LED_Pin, GPIO_PIN_RESET);

		HAL_ADC_Start(hadc);

		HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);

		uint16_t v_temp = (uint16_t) HAL_ADC_GetValue(hadc);

		HAL_ADC_Stop(hadc);

		if (v_temp > adc_V0)
			adc_V0 = v_temp;

		TIM2_Delay_us(40);

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);

		HAL_Delay(10);
	}

	/*
	 *  Vmax(100%)=4V
	 *  R1=10k
	 *  R2=10k
	 *  k=2.0
	 *
	 */

	debug("\t...DONE\n\r");

	return (adc_V0 * 3.3f) / 4095.0f * 2.0f;

}

float get_infra_sensor_data(ADC_HandleTypeDef *hadc) {

	debug("Polling for dust sensor data...");

	HAL_Delay(100); // Sensor stabilization time (min 100ms)

	return poll_sensor(hadc, 3);
}
