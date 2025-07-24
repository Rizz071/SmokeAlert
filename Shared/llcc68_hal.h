#ifndef __LLCC68_HAL_H__
#define __LLCC68_HAL_H__

#include "main.h"

// 🧩 Функции управления
void LLCC68_Spi_Driver_Init(SPI_HandleTypeDef *hspi_extern,
		GPIO_TypeDef *GPIO_PORT_NRST, uint16_t GPIO_Pin_NRST,
		GPIO_TypeDef *GPIO_PORT_NSS, uint16_t GPIO_Pin_NSS,
		GPIO_TypeDef *GPIO_PORT_BUSY, uint16_t GPIO_Pin_BUSY);
void LLCC68_SetModulationParams();
void LLCC68_SetPacketParams(uint8_t payload_length);
void LLCC68_Reset(void);
void LLCC68_WaitWhileBusy(void);

// 📤 Команды SPI
void LLCC68_WriteCommand(uint8_t cmd, uint8_t *data, uint8_t len);
void LLCC68_ReadCommand(uint8_t cmd, uint8_t *data, uint8_t len);
void LLCC68_WriteRegister(uint16_t addr, uint8_t *data, uint8_t len);
uint16_t LLCC68_ReadIrqStatus();
void LLCC68_ClearAllIrqStatus();
void LLCC68_SetRfFrequency(uint32_t freq_hz);
uint8_t LLCC68_CheckStatus(void);
uint8_t LLCC68_WriteBuffer(uint8_t offset, uint8_t *data, uint8_t len);
uint8_t LLCC68_ReadBuffer(uint8_t offset, uint8_t *data, uint8_t len);

#endif // __LLCC68_HAL_H__
