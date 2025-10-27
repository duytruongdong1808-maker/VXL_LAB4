#ifndef __FSM_H__
#define __FSM_H__

#include "main.h"

extern int mode;
extern int prev_mode;
extern int temp_red_time;
extern int temp_amber_time;
extern int temp_green_time;


void fsm_traffic_run(void);

#endif /* __FSM_H__ */
