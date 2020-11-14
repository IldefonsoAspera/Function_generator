/*
 * app_uartTx.h
 *
 *  Created on: 12 nov. 2020
 *      Author: Ildefonso
 */

#ifndef INC_APP_UARTTX_H_
#define INC_APP_UARTTX_H_

#include "cmsis_os.h"

// TODO Use the definition instead of the number in the UART TX queue definition
#define UART_STRING_MAX_LENGTH	128		// Max number of characters that can be sent to the computer per print (Buffers size)


void uartPrint(char* string);
void uartTxISR_charSent_callback();

void uartPrintFromISR(char* string);
void processUartTx(osMessageQueueId_t uartTxQueueHandle);

#endif /* INC_APP_UARTTX_H_ */
