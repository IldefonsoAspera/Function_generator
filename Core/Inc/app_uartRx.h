/*
 * app_uartRx.h
 *
 *  Created on: 12 nov. 2020
 *      Author: Ildefonso
 */

#ifndef INC_APP_UARTRX_H_
#define INC_APP_UARTRX_H_


#define UART_INPUT_BUFFER_SIZE	32		// max number of characters that can be sent from the computer to the microcontroller




void procUartRxISR(uint8_t rcvdChar);
void processUartRx();


#endif /* INC_APP_UARTRX_H_ */
