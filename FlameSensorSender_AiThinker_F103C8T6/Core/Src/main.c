/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "spi.h"
#include <stdio.h>
#include "DebugLog.h"
#include "Types.h"
#include "AlarmReset.h"
#include "SerialNumber.h"
#include "BatteryLevel.h"
#include "InfraSensor.h"
#include "BackupReg.h"
#include <stdbool.h>
#include "DataStorage.h"
#include "LoraSender.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_RTC_Init();
	MX_SPI1_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */

	Settings_t settings;
	SerialNumber_t hw_serial;
	Battery_t Battery;
	SendPacket_t packet;

	bool ALERT_FLAG = false;

	debug_init(&huart1);

	debug("========================");
	debug("The device has woken up!");

	settings = retrieveSettingsFromFlash();

	debug("\tAlarm level: %d.%02d", (uint8_t) settings.alarm_level,
			(uint8_t) (settings.alarm_level * 100) % 100);
	debug("\tSleep time: %d sec", settings.sleep_time);
	debug("\tHeartbeat every: %d time\n\r", settings.times_to_heartbeat);

	MX_ADC1_Init();
	Battery = get_battery_level(&hadc1);
	HAL_ADC_DeInit(&hadc1);

	// Powering VCC to Smoke Sensor and polling for data
	MX_ADC2_Init();
	HAL_GPIO_WritePin(GPIOB, MOSFET_GATE_SENSOR_Pin, GPIO_PIN_SET);
	float sensor_data = get_infra_sensor_data(&hadc2, 1);
	if (sensor_data >= settings.alarm_level) {
		sensor_data = get_infra_sensor_data(&hadc2, 3);
		if (sensor_data >= settings.alarm_level)
			ALERT_FLAG = true;
	}
	HAL_GPIO_WritePin(GPIOB, MOSFET_GATE_SENSOR_Pin, GPIO_PIN_RESET);
	HAL_ADC_DeInit(&hadc2);

	init_backup_register();
	uint8_t backup_reg_current_value = read_backup_register();
	debug("Sleeps to heartbeat: %d of %d\n\r", backup_reg_current_value + 1,
			settings.times_to_heartbeat);

	if ((backup_reg_current_value + 1 >= settings.times_to_heartbeat)
			|| ALERT_FLAG) {

		hw_serial = get_serial_number();

		packet.ID = hw_serial.byte_2;
		packet.battery_level = Battery.charge_percent;
		packet.sensor_data = sensor_data;
		debug("Formed packet:");
		debug("\tID: %d", (uint8_t) packet.ID);
		debug("\tbattery_level: %d%%", (uint8_t) Battery.charge_percent);
		debug("\tsensor_data: %d.%02d\n\r", (uint8_t) sensor_data,
				(uint16_t) (sensor_data * 100) % 100);

		HAL_GPIO_WritePin(GPIOB, MOSFET_GATE_LORA_Pin, GPIO_PIN_SET);
		MX_SPI1_Init();
		send_packet(&hspi1, packet);
		HAL_GPIO_WritePin(GPIOB, MOSFET_GATE_LORA_Pin, GPIO_PIN_RESET);

		write_backup_register(0);
	} else {
		write_backup_register(++backup_reg_current_value);
	}

	if (ALERT_FLAG) {
		debug("\n\rBUZZ BUZZ BUZZ\n\r\n\r");

		HAL_GPIO_WritePin(GPIOB, MOSFET_GATE_SENSOR_Pin, GPIO_PIN_SET);
		for (int i = 0; i < 3; i++) {
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			HAL_Delay(500);
		}
		HAL_GPIO_WritePin(GPIOB, MOSFET_GATE_SENSOR_Pin, GPIO_PIN_RESET);

		settings.sleep_time = 5;
	}

	//  Toggling RED LED
	HAL_GPIO_WritePin(INFO_LED_GPIO_Port, INFO_LED_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(INFO_LED_GPIO_Port, INFO_LED_Pin, GPIO_PIN_RESET);

	// Sleeping
	set_alarm(&hrtc, settings.sleep_time);

	debug("Going STANDBY MODE\n\r\n\r");

	HAL_PWR_EnterSTANDBYMode();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI
			| RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
	// Просто заглушка — достаточно, чтобы выйти из Standby
	debug("RTC Alarm Callback triggered!\n\r");
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
