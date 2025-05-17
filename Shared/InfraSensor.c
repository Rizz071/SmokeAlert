/*
 * InfraSensor.c
 *
 *  Created on: May 5, 2025
 *      Author: rizz
 */

#include "InfraSensor.h"
#include "Delay_ns.h"

static ADC_HandleTypeDef *hadc;
static float alarm_level;

uint32_t poll_sensor(uint8_t times) {

	uint32_t adc_V0 = 0;

	for (uint8_t i = 1; i <= times; i++) {

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);

		TIM2_Delay_us(280);

		HAL_ADC_Start(hadc);

		HAL_ADC_PollForConversion(hadc, 10);

		uint32_t v_temp = HAL_ADC_GetValue(hadc);

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
	 *  k=0.5
	 *
	 */

	return adc_V0 / 4095.0f * 3.3f;

}

float get_infra_sensor_data(ADC_HandleTypeDef *ha, float alarm_lvl) {

	hadc = ha;
	alarm_level = alarm_lvl;

	// Sensor stabilization for 100ms
	HAL_Delay(200);

	uint32_t real_adc_V0;

	real_adc_V0 = poll_sensor(1) * 2;

	// Recheck when alarm threshold is exceeded
	if (real_adc_V0 >= alarm_level) {
		real_adc_V0 = poll_sensor(3) * 2;
	}

	return (float) real_adc_V0;
}
