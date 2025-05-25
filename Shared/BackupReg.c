/*
 * BackupReg.c
 *
 *  Created on: May 25, 2025
 *      Author: rizz
 */

#include "BackupReg.h"
#include "stm32f1xx_hal.h"

void init_backup_register() {
	__HAL_RCC_BKP_CLK_ENABLE();
}

uint8_t read_backup_register() {

	if ((BKP->DR1 & 0xFF00) != 0xA500) {

		BKP->DR1 = (0xA5 << 8) | 0x00;
	}

	return BKP->DR1 & 0x00FF;
}

void write_backup_register(uint8_t value) {

	BKP->DR1 = (0xA5 << 8) | value;
}
