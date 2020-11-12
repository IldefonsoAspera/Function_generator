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


static osMessageQueueId_t m_uartTxQueueHandle;

BaseType_t uartPrint(char* string)
{
	char printBuffer[UART_STRING_MAX_LENGTH];

	strncpy(printBuffer, string, UART_STRING_MAX_LENGTH);
	return xQueueSendToBack(m_uartTxQueueHandle, printBuffer, 0);
}

// TODO fix case when passed string is smaller than max length and does not have a null terminator
BaseType_t uartPrintFromISR(char* string)
{
	char printBuffer[UART_STRING_MAX_LENGTH];

	strncpy(printBuffer, string, UART_STRING_MAX_LENGTH);
	return xQueueSendToBackFromISR(m_uartTxQueueHandle, printBuffer, 0);
}


void processUartTx(osMessageQueueId_t uartTxQueueHandle)
{
	char txBuffer[UART_STRING_MAX_LENGTH+1];
	uint32_t i, length;

	m_uartTxQueueHandle = uartTxQueueHandle;
	for(;;)
	{
		if(xQueueReceive(m_uartTxQueueHandle, txBuffer, portMAX_DELAY) == pdTRUE)
		{
			length = strnlen(txBuffer, UART_STRING_MAX_LENGTH);
			txBuffer[length++] = '\n';
			for(i=0; i<length; i++)
			{
				while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
				LL_LPUART_TransmitData8(LPUART1, txBuffer[i]);
			}
		}
	}
}
