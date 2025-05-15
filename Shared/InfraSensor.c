/*
 * InfraSensor.c
 *
 *  Created on: May 5, 2025
 *      Author: rizz
 */

#include "InfraSensor.h"
//#include "stm32f1xx_hal.h"
#include "Delay_ns.h"
#include "main.h"

extern ADC_HandleTypeDef hadc2;

float get_infra_sensor_data() {

	//Sensor stabilization for 100ms
	HAL_Delay(200);

	uint32_t adc_V0 = 0;

	// Polling sensor 2 times
	for (int i = 0; i <= 1; i++) {



		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);

		TIM2_Delay_us(280);
		HAL_ADC_Start(&hadc2);
		HAL_ADC_PollForConversion(&hadc2, 10);

//		if (HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK) {
		uint32_t v_temp = HAL_ADC_GetValue(&hadc2);
		HAL_ADC_Stop(&hadc2);

		if (v_temp > adc_V0)
			adc_V0 = v_temp;
//		}

		TIM2_Delay_us(40);


		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);

		TIM2_Delay_us(10000);
	}

	/*
	 *  Vmax(100%)=3.6V
	 *  R1=220
	 *  R2=330
	 *  k=1.67
	 *
	 */

	float sensor_data = 1.67 * adc_V0 * 3.6 / 4096.0f;

	return sensor_data;
}
