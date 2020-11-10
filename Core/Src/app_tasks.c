/*
 * app_tasks.c
 *
 *  Created on: 8 nov. 2020
 *      Author: Ilde
 */
#include "main.h"
#include "cmsis_os.h"
#include "app_tasks.h"
#include "message_buffer.h"
#include <string.h>
#include "queue.h"



/******************************************* UART RX **************************************************/

static uint8_t               uartRxStorageBuffer[UART_INPUT_BUFFER_SIZE + 1];			// Message buffer for UART ISR
static StaticMessageBuffer_t uartRxMessageBufferStruct;
static MessageBufferHandle_t uartRxMessageBuffer;



// Function that will be called with each UART byte received
// After turning on or after a correct message (one that fits in the buffer),
// the function will store each character received. When a \n is received,
// it will insert the filled buffer in the msg stream to the processing task.
// If the buffer is full, it will instead wait for a \n to restart again.
void procUartRxISR(uint8_t rcvdChar)
{
	static char     buffer[UART_INPUT_BUFFER_SIZE] = {0};
	static uint32_t nBuf = 0;
	size_t          xBytesSent;
	BaseType_t      xHigherPriorityTaskWoken = pdFALSE;

	if(rcvdChar == '\n' && nBuf == UART_INPUT_BUFFER_SIZE)
	{
		uartPrintFromISR("E: Received string is too long");
		nBuf = 0;
	}
	else if(rcvdChar == '\n' && nBuf != 0)
	{
		uartPrintFromISR("D: Added string in ISR to msg buffer, received from UART:");
		uartPrintFromISR(buffer);

		xBytesSent = xMessageBufferSendFromISR(uartRxMessageBuffer, buffer, nBuf, &xHigherPriorityTaskWoken);
		if(xBytesSent != nBuf)
			uartPrintFromISR("E: UART RX ISR message buffer overflowed");
		memset(buffer, 0, sizeof(buffer));	// TODO replace with a better way to uartPrint with null terminators
		nBuf = 0;
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
	else if(nBuf != UART_INPUT_BUFFER_SIZE)
	{
		if(rcvdChar != '\r')
			buffer[nBuf++] = rcvdChar;
	}
}


void processUartRx()
{
	char uartRxTaskBuffer[UART_INPUT_BUFFER_SIZE] = {0};

	// Enable RXNE and Error interrupts
	LL_LPUART_EnableIT_RXNE(LPUART1);
	LL_LPUART_EnableIT_ERROR(LPUART1);

	// Create message buffer
	uartRxMessageBuffer = xMessageBufferCreateStatic(sizeof(uartRxStorageBuffer), uartRxStorageBuffer, &uartRxMessageBufferStruct);

	for(;;)
	{
		if(xMessageBufferReceive(uartRxMessageBuffer, uartRxTaskBuffer, UART_INPUT_BUFFER_SIZE, portMAX_DELAY) != 0)
		{
			uartPrint("D: Received from UART RX message buffer:");
			uartPrint(uartRxTaskBuffer);
			memset(uartRxTaskBuffer, 0, sizeof(uartRxTaskBuffer));	// TODO replace with a better way to uartPrint with null terminators
		}
	}
}
/***************************************** END UART RX ************************************************/



/******************************************* UART TX **************************************************/

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
/***************************************** END UART TX ************************************************/



void processNewSignal()
{
	/* Infinite loop */
	for(;;)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(50);
	}
}
