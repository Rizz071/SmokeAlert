/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define V0_Pin GPIO_PIN_0
#define V0_GPIO_Port GPIOA
#define VCC_BAT_Pin GPIO_PIN_1
#define VCC_BAT_GPIO_Port GPIOA
#define NSS_SIGNAL_Pin GPIO_PIN_4
#define NSS_SIGNAL_GPIO_Port GPIOA
#define DIO0_INT_Pin GPIO_PIN_0
#define DIO0_INT_GPIO_Port GPIOB
#define RST_SIGNAL_Pin GPIO_PIN_1
#define RST_SIGNAL_GPIO_Port GPIOB
#define MOSFET_GATE_SENSOR_Pin GPIO_PIN_10
#define MOSFET_GATE_SENSOR_GPIO_Port GPIOB
#define MOSFET_GATE_LORA_Pin GPIO_PIN_11
#define MOSFET_GATE_LORA_GPIO_Port GPIOB
#define V_LED_Pin GPIO_PIN_12
#define V_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
