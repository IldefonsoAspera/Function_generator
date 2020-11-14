/*
 * app_uartTx.c
 *
 *  Created on: 12 nov. 2020
 *      Author: Ildefonso
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "cmsis_gcc.h"
#include "main.h"


// Buffer size must be a power of two to optimize operations
#define FIFO_SIZE_EXP	10
#define FIFO_SIZE		(1<<FIFO_SIZE_EXP)

static char uartTxFifo[FIFO_SIZE];
static int  rdOff = 0; 					// Last read item
static int  wrOff = 0; 					// Next item to write
static bool isTx  = false;


static char fifo_get()
{
	char retVal = '\0';
	if(wrOff-rdOff != 0)
		retVal = uartTxFifo[rdOff++ & (FIFO_SIZE-1)];
	else
		retVal = '\0';
	return retVal;
}


// To be used to start TX and in ISR to continue transmission
static void uartTx_sendNext()
{
	char newChar = fifo_get();
	if(newChar != '\0'){
		LL_LPUART_TransmitData8(LPUART1, newChar);
		isTx = true;
	}
	else{
		LL_LPUART_DisableIT_TXE_TXFNF(USART1);
		isTx = false;
	}
}


static void fifo_put(char* str)
{
	while(wrOff-rdOff <= FIFO_SIZE && *str != '\0')
		uartTxFifo[wrOff++ & (FIFO_SIZE-1)] = *str++;
}


void uartPrint(char* str)
{
	__disable_irq();
	fifo_put(str);
	if(!isTx)
	{
		uartTx_sendNext();
		LL_LPUART_EnableIT_TXE_TXFNF(LPUART1);
	}
	__enable_irq();
}


void uartTxISR_charSent_callback()
{
	__disable_irq();
	uartTx_sendNext();
	__enable_irq();
}
