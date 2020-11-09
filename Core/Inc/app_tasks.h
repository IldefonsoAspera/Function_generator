/*
 * app_tasks.h
 *
 *  Created on: 8 nov. 2020
 *      Author: Ilde
 */

#ifndef INC_APP_TASKS_H_
#define INC_APP_TASKS_H_


#define UART_INPUT_BUFFER_SIZE	32




void procUartRxISR(uint8_t rcvd_data);


void processUartRx();
void processUartTx();
void processNewSignal();








#endif /* INC_APP_TASKS_H_ */
