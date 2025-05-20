/*
 * AlarmReset.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "AlarmReset.h"
#include "main.h"
#include "DebugLog.h"

void set_alarm(RTC_HandleTypeDef *hrtc, uint16_t sec) {

	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
		// MCU проснулся из Standby
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

		// ОЧИСТИТЬ БУДИЛЬНИК
		HAL_RTC_DeactivateAlarm(hrtc, RTC_ALARM_A);
	}

	RTC_AlarmTypeDef sAlarm = { 0 };

	sAlarm.AlarmTime.Hours = 0;
	sAlarm.AlarmTime.Minutes = 0;
	sAlarm.AlarmTime.Seconds = sec;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	debug("Wake Up timer setup for %d sec\n\r", sec);

}

