/*
 * app_tasks.h
 *
 *  Created on: 8 nov. 2020
 *      Author: Ilde
 */

#ifndef INC_APP_FUNCGEN_H_
#define INC_APP_FUNCGEN_H_


typedef enum {FUNC_SINE} funcType;

typedef struct {
	funcType type;
	uint32_t freq;
} funcParams;




void addNewSignal(funcParams newParams);
void processNewSignal(TIM_HandleTypeDef *htim2, DAC_HandleTypeDef *hdac1);



#endif /* INC_APP_FUNCGEN_H_ */
