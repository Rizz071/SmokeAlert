/*
 * AlarmReset.h
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#ifndef INC_ALARMRESET_H_
#define INC_ALARMRESET_H_

#include "stm32f1xx_hal.h"

void set_alarm(RTC_HandleTypeDef *hrtc, uint16_t sec);

#endif /* INC_ALARMRESET_H_ */
