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
#include "i2c.h"
#include "icache.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "llcc68_hal.h"
#include "LoraReceiver.h"
#include "callbacks.h"
#include "DebugLog.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
//#include "ssd1306_tests.h"

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
	MX_ICACHE_Init();
	MX_SPI1_Init();
	MX_LPUART1_UART_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	debug_init(&hlpuart1);

	ssd1306_Init();
	ssd1306_Fill(Black);

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("RECEIVER", Font_7x10, White);

	ssd1306_SetCursor(0, 27);
	ssd1306_WriteString("Smoke: ", Font_7x10, White);

	ssd1306_UpdateScreen();

	LLCC68_Spi_Driver_Init(&hspi1, LORA_NRST_GPIO_Port, LORA_NRST_Pin,
	LORA_NSS_GPIO_Port,
	LORA_NSS_Pin, LORA_BUSY_GPIO_Port, LORA_BUSY_Pin);

	LLCC68_Reset();

	uint8_t status = LLCC68_CheckStatus();

	if ((status & 0xF0) != 0x00) {
		// Модуль ответил статусом => жив

		HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
				GPIO_PIN_RESET); // светодиод ВЫКЛ (Bluepill style)
	} else {

		// Статус = 0x00 => модуль не отвечает
		HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
				GPIO_PIN_SET);   // светодиод ВКЛ
	}

	// Set Standby mode
	uint8_t standby_param = 0x00; // RC
	LLCC68_WriteCommand(0x80, &standby_param, 1);

	// Set Packet Type to LoRa
	uint8_t packet_type = 0x01;
	LLCC68_WriteCommand(0x8A, &packet_type, 1);

	// Set RF frequency
	LLCC68_SetRfFrequency(870000000);

	// Set buffer base address
	uint8_t base_addr[2] = { 0x00, 0x00 };  // TX = 0x00, RX = 0x00
	LLCC68_WriteCommand(0x8F, base_addr, 2);

	// Настройка параметров модуляции LoRa
	// Параметры задаются в драйвере LLCC68 сразу для всех устройств
	LLCC68_SetModulationParams();

	// Настройка параметров пакета
	LLCC68_SetPacketParams(0x7F);	// 127 байт максимально

	// Set all interrupts to DIO1
	uint8_t params3[8];
	uint16_t irqMask = 0x00FF;    // включаем IRQ биты 0-7
	uint16_t dio1Mask = 0x00FF;   // все эти IRQ выводим на DIO1
	uint16_t dio2Mask = 0x0000;   // DIO2 ничего не выводит
	uint16_t dio3Mask = 0x0000;   // DIO3 ничего не выводит
	params3[0] = (irqMask >> 8) & 0xFF;
	params3[1] = irqMask & 0xFF;
	params3[2] = (dio1Mask >> 8) & 0xFF;
	params3[3] = dio1Mask & 0xFF;
	params3[4] = (dio2Mask >> 8) & 0xFF;
	params3[5] = dio2Mask & 0xFF;
	params3[6] = (dio3Mask >> 8) & 0xFF;
	params3[7] = dio3Mask & 0xFF;
	LLCC68_WriteCommand(0x08, params3, 8);

	// Включение режима приёма (RX)
	uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
	LLCC68_WriteCommand(0x82, rx_timeout, 3);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		// Проверка статуса IRQ
		uint8_t irq_status[2];
		LLCC68_ReadCommand(0x12, irq_status, 2); // GetIrqStatus

		if (irq_status[1] & 0x02) { // RxDone

			HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
					GPIO_PIN_SET);
			HAL_Delay(100);
			HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
					GPIO_PIN_RESET);

			// Чтение длины принятого пакета
			uint8_t rx_info[2];
			LLCC68_ReadCommand(0x13, rx_info, 2); // GetRxBufferStatus
			uint8_t payload_len = rx_info[0];
			uint8_t buffer_offset = rx_info[1];

			// Чтение данных из буфера
			uint8_t rx_data[payload_len]; // Буфер для данных

			LLCC68_ReadBuffer(buffer_offset, rx_data, payload_len);

			SendPacket_t packet;
			memcpy(&packet, &rx_data, sizeof(SendPacket_t));

			char text_out[32];
			sprintf(text_out, "%.2f", packet.sensor_data);

			ssd1306_SetCursor(50, 20);
			ssd1306_WriteString(text_out, Font_11x18, White);

			ssd1306_UpdateScreen();

			// Возврат в режим приёма
			LLCC68_WriteCommand(0x82, rx_timeout, 3);
		}

		HAL_Delay(300); // Небольшая задержка для снижения нагрузки на CPU

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

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE
			| RCC_OSCILLATORTYPE_CSI;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.CSIState = RCC_CSI_ON;
	RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 2;
	RCC_OscInitStruct.PLL.PLLN = 11;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
		Error_Handler();
	}

	/** Configure the programming delay
	 */
	__HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_1);
}

/* USER CODE BEGIN 4 */

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
