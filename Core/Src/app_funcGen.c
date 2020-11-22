/*
 * app_tasks.c
 *
 *  Created on: 8 nov. 2020
 *      Author: Ilde
 */
#include <stdint.h>
#include "main.h"
#include "cmsis_os.h"
#include "message_buffer.h"
#include "app_funcGen.h"
#include "app_uartRx.h"
#include "app_uartTx.h"
#include "app_log.h"

static uint8_t               funcGenStorageBuffer[sizeof(funcParams) * 10];
static StaticMessageBuffer_t funcGenMessageBufferStruct;
static MessageBufferHandle_t funcGenMessageBuffer;

/* Sine wave values for a complete symbol */
static uint16_t sinewave[60] = {
		0x07ff,0x08cb,0x0994,0x0a5a,0x0b18,0x0bce,0x0c79,0x0d18,0x0da8,0x0e29,0x0e98,0x0ef4,0x0f3e,0x0f72,0x0f92,0x0f9d,
		0x0f92,0x0f72,0x0f3e,0x0ef4,0x0e98,0x0e29,0x0da8,0x0d18,0x0c79,0x0bce,0x0b18,0x0a5a,0x0994,0x08cb,0x07ff,0x0733,
		0x066a,0x05a4,0x04e6,0x0430,0x0385,0x02e6,0x0256,0x01d5,0x0166,0x010a,0x00c0,0x008c,0x006c,0x0061,0x006c,0x008c,
		0x00c0,0x010a,0x0166,0x01d5,0x0256,0x02e6,0x0385,0x0430,0x04e6,0x05a4,0x066a,0x0733};


void addNewSignal(funcParams newParams)
{
	size_t xBytesSent;

	xBytesSent = xMessageBufferSend(funcGenMessageBuffer, &newParams, sizeof(newParams), portMAX_DELAY);
	if(xBytesSent != sizeof(newParams))
		log_error("New pattern message buffer overflowed");
	else
		log_debug("New pattern added to message buffer");
}


void processNewSignal(TIM_HandleTypeDef *htim2, DAC_HandleTypeDef *hdac1)
{
	funcParams newParams;

	APP_CHECK_HAL (HAL_DAC_Start_DMA(hdac1, DAC_CHANNEL_1, (uint32_t *)sinewave, 60, DAC_ALIGN_12B_R));		/* DAC conversion start error */
	APP_CHECK_HAL (HAL_TIM_Base_Start(htim2));																/* Counter enable error */

	// Create message buffer
	funcGenMessageBuffer = xMessageBufferCreateStatic(sizeof(funcGenStorageBuffer), funcGenStorageBuffer, &funcGenMessageBufferStruct);

	log_debug("Started function generator task");

	for(;;)
	{
		if(xMessageBufferReceive(funcGenMessageBuffer, &newParams, sizeof(newParams), portMAX_DELAY) != 0)
		{
			log_debug("New function, freq: %i", newParams.freq);
			APP_CHECK_HAL (HAL_TIM_Base_Stop(htim2));
			htim2->Init.Period = 160000000UL / ((uint64_t)newParams.freq*60);
			APP_CHECK_HAL (HAL_TIM_Base_Init(htim2));
			APP_CHECK_HAL (HAL_TIM_Base_Start(htim2));
		}

	}
}
