/*
 * scheduler.h
 *
 *  Created on: Oct 24, 2025
 *      Author: truon
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"


#define IDX_INVALID ((uint8_t)0xFF)

#define ERROR_HOLD_TICKS 6000
#define SCH_MAX_TASKS 40
#define NO_TASK_ID 0

#define ERROR_SCH_TOO_MANY_TASKS                        1
#define ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK              2
#define ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER 3
#define ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START      4
#define ERROR_SCH_LOST_SLAVE                            5
#define ERROR_SCH_CAN_BUS_ERROR                         6
#define ERROR_I2C_WRITE_BYTE_AT24C64                    7

typedef struct {
    void (*pTask)(void);
    uint32_t Delay;
    uint32_t Period;
    uint8_t  RunMe;
    uint32_t TaskID;

    uint8_t  next;
	uint8_t  prev;
	uint8_t  in_use;
} sTask;

extern unsigned char Error_code_G;
extern unsigned char Last_error_code_G;
extern unsigned int  Error_tick_count_G;

void SCH_Init(void);

void SCH_Update(void);

unsigned char SCH_Add_Task ( void (*pFunction)(), unsigned int DELAY, unsigned int PERIOD );

unsigned char SCH_Delete_Task (const unsigned char TASK_INDEX);

void SCH_Dispatch_Tasks(void);

void SCH_Go_To_Sleep(void);

void SCH_Report_Status(void);

#endif /* INC_SCHEDULER_H_ */
