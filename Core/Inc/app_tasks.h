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


typedef enum {FUNC_SINE, FUNC_RAMP, FUNC_SQR, FUNC_TRI} funcType;

typedef struct {
	funcType type;
	uint32_t freq;
} funcParams;







BaseType_t uartPrint(char* string);
BaseType_t uartPrintFromISR(char* string);

void procUartRxISR(uint8_t rcvdChar);
void processUartRx();
void processUartTx(osMessageQueueId_t uartTxQueueHandle);
void processNewSignal(TIM_HandleTypeDef *htim2, DAC_HandleTypeDef *hdac1);








#endif /* INC_APP_TASKS_H_ */
