/*
 * watchdog.h
 *
 *  Created on: Oct 24, 2025
 *      Author: truon
 */

#ifndef INC_WATCHDOG_H_
#define INC_WATCHDOG_H_

#include "main.h"

void MX_IWDG_Init(void);
void Watchdog_Refresh(void);
unsigned char Is_Watchdog_Reset(void);
void Watchdog_Counting(void);
void Reset_Watchdog_Counting(void);

#endif /* INC_WATCHDOG_H_ */
