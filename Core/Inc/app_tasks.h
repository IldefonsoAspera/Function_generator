/*
 * app_tasks.h
 *
 *  Created on: 8 nov. 2020
 *      Author: Ilde
 */

#ifndef INC_APP_TASKS_H_
#define INC_APP_TASKS_H_


#include "cmsis_os.h"

// TODO Use the definition instead of the number in the UART TX queue definition
#define UART_STRING_MAX_LENGTH	128		// Max number of characters that can be sent to the computer per print (Buffers size)
#define UART_INPUT_BUFFER_SIZE	32		// max number of characters that can be sent from the computer to the microcontroller


BaseType_t uartPrint(char* string);
BaseType_t uartPrintFromISR(char* string);

void procUartRxISR(uint8_t rcvdChar);
void processUartRx();
void processUartTx(osMessageQueueId_t uartTxQueueHandle);
void processNewSignal();








#endif /* INC_APP_TASKS_H_ */
