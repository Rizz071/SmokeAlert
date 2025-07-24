/*
 * DataStorage.h
 *
 *  Created on: Jun 24, 2025
 *      Author: rizz
 */

#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_

#include "Types.h"

int storeSettingsToFlash(Settings_t*);
Settings_t retrieveSettingsFromFlash();

#endif /* DATASTORAGE_H_ */
