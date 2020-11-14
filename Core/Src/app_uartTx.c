/*
 * app_uartTx.c
 *
 *  Created on: 12 nov. 2020
 *      Author: Ildefonso
 */

#include "cmsis_os.h"
#include "app_uartTx.h"
#include <string.h>
#include "main.h"
#include "queue.h"
#include "app_log.h"

static osMessageQueueId_t m_uartTxQueueHandle;

void uartPrint(char* string)
{
	char printBuffer[UART_STRING_MAX_LENGTH];

	strncpy(printBuffer, string, UART_STRING_MAX_LENGTH);
	xQueueSendToBack(m_uartTxQueueHandle, printBuffer, 0);
}

// TODO fix case when passed string is smaller than max length and does not have a null terminator
void uartPrintFromISR(char* string)
{
	char printBuffer[UART_STRING_MAX_LENGTH];

	strncpy(printBuffer, string, UART_STRING_MAX_LENGTH);
	xQueueSendToBackFromISR(m_uartTxQueueHandle, printBuffer, 0);
}


void processUartTx(osMessageQueueId_t uartTxQueueHandle)
{
	char txBuffer[UART_STRING_MAX_LENGTH];
	uint32_t i, length;

	m_uartTxQueueHandle = uartTxQueueHandle;

	log_debug("debug");
	log_info("information");
	log_notif("notification");
	log_warn("warning");
	log_error("error");
	for(;;)
	{
		if(xQueueReceive(m_uartTxQueueHandle, txBuffer, portMAX_DELAY) == pdTRUE)
		{
			length = strnlen(txBuffer, UART_STRING_MAX_LENGTH);
			for(i=0; i<length; i++)
			{
				while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
				LL_LPUART_TransmitData8(LPUART1, txBuffer[i]);
			}
		}
	}
}
