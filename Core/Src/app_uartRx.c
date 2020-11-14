/*
 * app_uartRx.c
 *
 *  Created on: 12 nov. 2020
 *      Author: Ildefonso
 */


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"
#include "message_buffer.h"
#include "main.h"
#include "app_uartRx.h"
#include "app_uartTx.h"
#include "app_funcGen.h"
#include "app_log.h"

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
	static char     buffer[UART_INPUT_BUFFER_SIZE+1];
	static uint32_t nBuf = 0;
	size_t          xBytesSent;
	BaseType_t      xHigherPriorityTaskWoken = pdFALSE;

	if((rcvdChar == '\n' || rcvdChar == '\r') && nBuf == UART_INPUT_BUFFER_SIZE)
	{
		log_error("Received string is too long");
		nBuf = 0;
	}
	else if((rcvdChar == '\n' || rcvdChar == '\r') && nBuf != 0)
	{
		buffer[nBuf++] = '\0';
		xBytesSent = xMessageBufferSendFromISR(uartRxMessageBuffer, buffer, nBuf, &xHigherPriorityTaskWoken);
		if(xBytesSent != nBuf)
			log_error("UART RX ISR message buffer overflowed");
		nBuf = 0;
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
	else if(nBuf != UART_INPUT_BUFFER_SIZE)
	{
		buffer[nBuf++] = rcvdChar;
	}
}


static void decodeNewSignalParams(char* string, size_t length, funcParams *p_newParams)
{
	if(strnstr(string, "sine", length))	         p_newParams->type = FUNC_SINE;
	else if(strnstr(string, "triangle", length)) p_newParams->type = FUNC_TRI;
	else if(strnstr(string, "square", length))   p_newParams->type = FUNC_SQR;
	else if(strnstr(string, "ramp", length))     p_newParams->type = FUNC_RAMP;
	else if(strnstr(string, "help", length))
	{
		uartPrint("Format: <signal type> <frequency>");
		uartPrint("<signal type>: sine, triangle, square, ramp");
		uartPrint("<frequency>: positive integer number from 1 to 2000000");
		uartPrint("example: \"sine 1000\" (1KHz sinusoidal)");
		return;
	}
	else{
		log_error("UART RX task, erroneous signal type");
		return;
	}


	char* numPos = strnstr(string, " ", length);
	int number   = atoi(numPos);
	p_newParams->freq = number;
}


void processUartRx()
{
	char uartRxTaskBuffer[UART_INPUT_BUFFER_SIZE] = {0};
	size_t strLength;
	funcParams newParams;

	// Create message buffer
	uartRxMessageBuffer = xMessageBufferCreateStatic(sizeof(uartRxStorageBuffer), uartRxStorageBuffer, &uartRxMessageBufferStruct);

	LL_LPUART_EnableIT_RXNE(LPUART1);
	for(;;)
	{
		if((strLength = xMessageBufferReceive(uartRxMessageBuffer, uartRxTaskBuffer, UART_INPUT_BUFFER_SIZE, portMAX_DELAY)) != 0)
		{
			decodeNewSignalParams(uartRxTaskBuffer, strLength, &newParams);
			addNewSignal(newParams);
		}
	}
}
