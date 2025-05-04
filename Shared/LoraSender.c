/*
 * LoraSender.c
 *
 *  Created on: May 3, 2025
 *      Author: rizz
 */

#include "stm32f1xx_hal.h"
#include "LoraSender.h"
#include "LoRa.h"
#include "BatteryLevel.h"
#include "Types.h"
#include "SerialNumber.h"

extern SPI_HandleTypeDef hspi1;
extern ADC_HandleTypeDef hadc2;

static LoRa myLoRa;
static SerialNumber_t Serial;

static void init_LoRa() {
	myLoRa = newLoRa();

	Serial = get_serial_number();

	myLoRa.CS_port = NSS_SIGNAL_GPIO_Port;
	myLoRa.CS_pin = NSS_SIGNAL_Pin;
	myLoRa.reset_port = RST_SIGNAL_GPIO_Port;
	myLoRa.reset_pin = RST_SIGNAL_Pin;
	myLoRa.DIO0_port = DIO0_INT_GPIO_Port;
	myLoRa.DIO0_pin = DIO0_INT_Pin;
	myLoRa.hSPIx = &hspi1;

	myLoRa.frequency = 433;             // default = 433 MHz
	//	myLoRa.spredingFactor = SF_7;            // default = SF_7
	//	myLoRa.bandWidth = BW_7_8KHz;       // default = BW_125KHz
	myLoRa.crcRate = CR_4_8;          // default = CR_4_5
	myLoRa.power = POWER_17db;      // default = 20db
	myLoRa.overCurrentProtection = 130;             // default = 100 mA
	myLoRa.preamble = 12;              // default = 8;

	while (LoRa_init(&myLoRa) != 200)
		;
}

void send_packet() {
	init_LoRa();

	SendPacket_t packet;

	uint32_t adc_V0 = 0;
	for (int i = 0; i <= 1; i++) {
		HAL_ADC_Start(&hadc2);

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_SET);

		HAL_Delay(28);

		if (HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK) {
			uint32_t v_temp = HAL_ADC_GetValue(&hadc2);
			if (v_temp > adc_V0)
				adc_V0 = v_temp;
		}

		HAL_Delay(12);

		HAL_ADC_Stop(&hadc2);

		HAL_GPIO_WritePin(V_LED_GPIO_Port, V_LED_Pin, GPIO_PIN_RESET);

	}

//	R1=220 Ohm
//	R2=330 Ohm
//	3.6V -> 2.16V
//  k=1.67

	Battery_t Battery = get_battery_level();

	float Vin = adc_V0 * (Battery.vdd / 4096.0f);
	float sensor_data = Vin * 2.5f;


//	uint16_t V0_result = 170 * adc_V0 * (3.0f / 4096.0f) - 0.1;

	packet.ID = Serial.byte_2;
	packet.sensor_data = sensor_data;
	packet.battery_level = Battery.charge_percent;

	LoRa_transmit(&myLoRa, (uint8_t*) &packet, sizeof(SendPacket_t), 100);
}
