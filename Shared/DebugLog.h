/*
 * DebugLog.h
 *
 *  Created on: May 17, 2025
 *      Author: rizz
 */

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_

void debug_init(UART_HandleTypeDef *hu);
void debug(const char *format, ...);

#endif /* DEBUGLOG_H_ */
