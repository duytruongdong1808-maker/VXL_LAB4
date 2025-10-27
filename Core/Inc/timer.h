/*
 * timer.h
 *
 *  Created on: Oct 24, 2025
 *      Author: truon
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"

void TIMER_Init(void);
void TIMER_Start(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* INC_TIMER_H_ */
