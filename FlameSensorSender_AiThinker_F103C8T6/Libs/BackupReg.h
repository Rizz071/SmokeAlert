/*
 * BackupReg.h
 *
 *  Created on: May 25, 2025
 *      Author: rizz
 */

#ifndef BACKUPREG_H_
#define BACKUPREG_H_

#include <stdint.h>

void init_backup_register();
uint8_t read_backup_register();
void write_backup_register(uint8_t value);

#endif /* BACKUPREG_H_ */
