/*
 * app_tasks.c
 *
 *  Created on: 8 nov. 2020
 *      Author: Ilde
 */
#include "main.h"
#include "cmsis_os.h"



// Test to check that received data is sent back correctly
void procUartRxISR(uint8_t rcvd_data)
{
	static char inBuf[UART_INPUT_BUFFER_SIZE];
	static uint32_t charCounter = 0;
	static bool waitingForNewLine = false;


	if(!waitingForNewLine){
		if(charCounter < UART_INPUT_BUFFER_SIZE-1){
			inBuf[charCounter++] = rcvd_data;
		}




	} else {


	}




	/* Wait for TXE flag to be raised */
	while (!LL_LPUART_IsActiveFlag_TXE(LPUART1))
	{
	}

	/* Write character in Transmit Data register.
       TXE flag is cleared by writing data in TDR register */
	txUartChar = rcvd_data;
	LL_LPUART_TransmitData8(LPUART1, rcvd_data);
}


void processUartRx()
{

	/* Clear Overrun flag, in case characters have already been sent to USART */
	LL_LPUART_ClearFlag_ORE(LPUART1);

	/* Enable RXNE and Error interrupts */
	LL_LPUART_EnableIT_RXNE(LPUART1);
	LL_LPUART_EnableIT_ERROR(LPUART1);
	/* Infinite loop */
	for(;;)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		osDelay(50);
	}

}


void processUartTx()
{
	/* Infinite loop */
	for(;;)
	{
		//HAL_UART_Transmit(&hlpuart1, (uint8_t*)"TEST_STRING\n", strlen("TEST_STRING\n"), HAL_MAX_DELAY);
		osDelay(1000);
	}

}


void processNewSignal()
{
	/* Infinite loop */
	for(;;)
	{
		osDelay(1);
	}
}
