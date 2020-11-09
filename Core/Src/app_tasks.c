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





/******************************************* UART RX **************************************************/

static uint8_t        uartRxStorageBuffer[UART_INPUT_BUFFER_SIZE + 1];			// Message buffer for UART ISR
StaticMessageBuffer_t uartRxMessageBufferStruct;
MessageBufferHandle_t uartRxMessageBuffer;

// Function that will be called with each UART byte received
// After turning on or after a correct message (one that fits in the buffer),
// the function will store each character received. When a \n is received,
// it will insert the filled buffer in the msg stream to the processing task.
// If buffer is full, it will instead wait for a \n to start filling it again.
void procUartRxISR(uint8_t rcvdChar)
{
	static char     buffer[UART_INPUT_BUFFER_SIZE];
	static uint32_t nBuf = 0;
	size_t          xBytesSent;
	BaseType_t      xHigherPriorityTaskWoken = pdFALSE;

	// If char is \n and buffer was full, notify user error and reset buffer
	// If char is \n and buffer is not full nor empty, add to msg queue
	// If char is not \n nor \r and buffer is not full, add to buffer
	if(rcvdChar == '\n' && nBuf == UART_INPUT_BUFFER_SIZE)
	{
		// TODO insert in TX queue error about incorrect length
		nBuf = 0;
	}
	else if(rcvdChar == '\n' && nBuf != 0)
	{
		xBytesSent = xMessageBufferSendFromISR(uartRxMessageBuffer, (void*)buffer, nBuf, &xHigherPriorityTaskWoken);
		if(xBytesSent != nBuf)
			while(1);				// TODO stream buffer is not big enough
		//taskYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		nBuf = 0;
	}
	else if(nBuf != UART_INPUT_BUFFER_SIZE)
	{
		if(rcvdChar != '\r')
			buffer[nBuf++] = rcvdChar;
	}






	// Wait for TXE flag to be raised
	while (!LL_LPUART_IsActiveFlag_TXE(LPUART1))
	{
	}

	// Write character in Transmit Data register.
	// TXE flag is cleared by writing data in TDR register
	LL_LPUART_TransmitData8(LPUART1, rcvdChar);
}


void processUartRx()
{
	// Enable RXNE and Error interrupts
	LL_LPUART_EnableIT_RXNE(LPUART1);
	LL_LPUART_EnableIT_ERROR(LPUART1);

	// Create message buffer
	uartRxMessageBuffer = xMessageBufferCreateStatic(sizeof(uartRxStorageBuffer), uartRxStorageBuffer, &uartRxMessageBufferStruct);

	for(;;)
	{
		osDelay(50);
	}
}
/***************************************** END UART RX ************************************************/

/******************************************* UART TX **************************************************/


void processUartTx()
{
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
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
