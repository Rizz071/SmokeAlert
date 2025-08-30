/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : app_freertos.c
 * Description        : FreeRTOS applicative file
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "llcc68_hal.h"
#include "LoraReceiver.h"
#include "callbacks.h"
#include "DebugLog.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "esp32c3.h"
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
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef hlpuart1;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;

char text_out[32] = "-1";

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = { .name = "defaultTask",
		.priority = (osPriority_t) osPriorityNormal, .stack_size = 400 * 4 };
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = { .name = "DisplayTask",
		.priority = (osPriority_t) osPriorityNormal, .stack_size = 256 * 4 };
/* Definitions for LoraReceiverTask */
osThreadId_t LoraReceiverTaskHandle;
const osThreadAttr_t LoraReceiverTask_attributes = { .name = "LoraReceiverTask",
		.priority = (osPriority_t) osPriorityNormal, .stack_size = 256 * 4 };
/* Definitions for esp32_uart_queue */
osMessageQueueId_t esp32_uart_queueHandle;
const osMessageQueueAttr_t esp32_uart_queue_attributes = { .name =
		"esp32_uart_queue" };
/* Definitions for displaySem */
osSemaphoreId_t displaySemHandle;
const osSemaphoreAttr_t displaySem_attributes = { .name = "displaySem" };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DisplayInit();
void LoraInit();

void LoraReceiverTask(void *argument);
//void DisplayTask(void *argument);
/* USER CODE END FunctionPrototypes */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
	/* USER CODE BEGIN Init */

	debug_init(&hlpuart1);

	DisplayInit();

	LoraInit();

//	esp32_init(&huart4, esp32_uart_queueHandle);

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */
	/* creation of displaySem */
	displaySemHandle = osSemaphoreNew(1, 1, &displaySem_attributes);

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */

	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */
	/* creation of esp32_uart_queue */
	esp32_uart_queueHandle = osMessageQueueNew(128, sizeof(uint8_t),
			&esp32_uart_queue_attributes);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */
	/* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
			&defaultTask_attributes);

	/* creation of DisplayTask */
	DisplayTaskHandle = osThreadNew(DisplayTask, NULL, &DisplayTask_attributes);

	/* creation of LoraReceiverTask */
	LoraReceiverTaskHandle = osThreadNew(LoraReceiverTask, NULL,
			&LoraReceiverTask_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */

	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief Function implementing the defaultTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
	/* USER CODE BEGIN defaultTask */

	esp32_init(&huart1, esp32_uart_queueHandle);

	// Тест подключения esp32 по UART
	char cmd1[] = "ATE0\r\n";
	esp32_send_AT(cmd1, sizeof(cmd1));

	// Тест подключения esp32 по UART
	char cmd2[] = "AT\r\n";
	esp32_send_AT(cmd2, sizeof(cmd2));

	/* Infinite loop */
	for (;;) {
		// for debug purposes

		osDelay(1000);
	}
	/* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_DisplayTask */
/**
 * @brief Function implementing the DisplayTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DisplayTask */
void DisplayTask(void *argument) {
	/* USER CODE BEGIN DisplayTask */
	/* Infinite loop */
	for (;;) {

		// Ждём уведомления семафором
		osSemaphoreAcquire(displaySemHandle, osWaitForever);

		ssd1306_SetCursor(50, 20);

		ssd1306_WriteString(text_out, Font_11x18, White);
		ssd1306_UpdateScreen();

	}
	/* USER CODE END DisplayTask */
}

/* USER CODE BEGIN Header_LoraReceiverTask */
/**
 * @brief Function implementing the LoraReceiverTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_LoraReceiverTask */
void LoraReceiverTask(void *argument) {
	/* USER CODE BEGIN LoraReceiverTask */
	/* Infinite loop */
	for (;;) {

		osDelay(10);

		// Проверка статуса IRQ
		uint8_t irq_status[2];
		LLCC68_ReadCommand(0x12, irq_status, 2); // GetIrqStatus
		LLCC68_ClearAllIrqStatus();

		if (irq_status[1] & 0x02) { // RxDone
			HAL_GPIO_WritePin(SMALL_INFO_LED_GPIO_Port, SMALL_INFO_LED_Pin,
					GPIO_PIN_SET);
			//			HAL_Delay(100);
			osDelay(100);

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

			sprintf(text_out, "%.2f", packet.sensor_data);

			osSemaphoreRelease(displaySemHandle); // разбудить задачу дисплея

			// Возврат в режим приёма
			uint8_t rx_timeout[3] = { 0xFF, 0xFF, 0xFF };
			LLCC68_WriteCommand(0x82, rx_timeout, 3);

//			osDelay(1);
		}
	}

	/* USER CODE END LoraReceiverTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void DisplayInit() {

	ssd1306_Init();
	ssd1306_Fill(Black);

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("RECEIVER", Font_7x10, White);

	ssd1306_SetCursor(0, 27);
	ssd1306_WriteString("Smoke: ", Font_7x10, White);

	ssd1306_UpdateScreen();
}

void LoraInit() {
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
}
/* USER CODE END Application */

