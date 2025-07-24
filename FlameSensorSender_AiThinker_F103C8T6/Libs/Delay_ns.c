/*
 * Delay_ns.c
 *
 *  Created on: May 5, 2025
 *      Author: rizz
 */

#include "Delay_ns.h"
#include <stdint.h>
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim2;

void TIM2_Delay_us(uint32_t us) {

	__HAL_TIM_SET_COUNTER(&htim2, 0);

	HAL_TIM_Base_Start(&htim2);

	while (__HAL_TIM_GET_COUNTER(&htim2) < us)
		;

	HAL_TIM_Base_Stop(&htim2);
}
