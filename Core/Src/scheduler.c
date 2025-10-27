#include "scheduler.h"
#include "watchdog.h"
#include "task.h"
#include "main.h"
#include <string.h>
sTask SCH_tasks_G[SCH_MAX_TASKS];

unsigned char Error_code_G = 0;
unsigned char Last_error_code_G = 0;
unsigned int Error_tick_count_G = 0;

static uint8_t g_delta_head = IDX_INVALID;
static uint8_t g_ready_head = IDX_INVALID;
static uint8_t g_free_head  = IDX_INVALID;

static void list_push_free(uint8_t idx) {
    SCH_tasks_G[idx].in_use = 0;
    SCH_tasks_G[idx].pTask  = 0;
    SCH_tasks_G[idx].Period = 0;
    SCH_tasks_G[idx].RunMe  = 0;
    SCH_tasks_G[idx].Delay  = 0;
    SCH_tasks_G[idx].prev   = IDX_INVALID;
    SCH_tasks_G[idx].next   = g_free_head;
    g_free_head = idx;
}

static uint8_t list_pop_free(void) {
    if (g_free_head == IDX_INVALID) return IDX_INVALID;
    uint8_t idx = g_free_head;
    g_free_head = SCH_tasks_G[idx].next;
    SCH_tasks_G[idx].next = IDX_INVALID;
    SCH_tasks_G[idx].in_use = 1;
    return idx;
}

static void ready_push(uint8_t idx) {
    SCH_tasks_G[idx].next = g_ready_head;
    SCH_tasks_G[idx].prev = IDX_INVALID;
    g_ready_head = idx;
}

static void delta_remove(uint8_t idx) {
    uint8_t prev = SCH_tasks_G[idx].prev;
    uint8_t next = SCH_tasks_G[idx].next;

    if (prev != IDX_INVALID) {
        SCH_tasks_G[prev].next = next;
    } else {
        g_delta_head = next;
    }
    if (next != IDX_INVALID) {
        SCH_tasks_G[next].prev = prev;
        SCH_tasks_G[next].Delay += SCH_tasks_G[idx].Delay;
    }

    SCH_tasks_G[idx].next = SCH_tasks_G[idx].prev = IDX_INVALID;
}

static void delta_insert_by_delay(uint8_t idx, uint32_t delay_ticks) {
    uint8_t cur = g_delta_head;
    uint8_t prev = IDX_INVALID;

    while (cur != IDX_INVALID && delay_ticks > SCH_tasks_G[cur].Delay) {
        delay_ticks -= SCH_tasks_G[cur].Delay;
        prev = cur;
        cur = SCH_tasks_G[cur].next;
    }

    SCH_tasks_G[idx].prev  = prev;
    SCH_tasks_G[idx].next  = cur;
    SCH_tasks_G[idx].Delay = delay_ticks;

    if (prev != IDX_INVALID) {
        SCH_tasks_G[prev].next = idx;
    } else {
        g_delta_head = idx;
    }

    if (cur != IDX_INVALID) {
        SCH_tasks_G[cur].prev = idx;
        SCH_tasks_G[cur].Delay -= delay_ticks;
    }
}


void SCH_Init(void) {
    unsigned char i;

    g_delta_head = g_ready_head = IDX_INVALID;
    g_free_head  = 0;

    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask  = 0;
        SCH_tasks_G[i].Delay  = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe  = 0;
        SCH_tasks_G[i].next   = (i + 1 < SCH_MAX_TASKS) ? (i + 1) : IDX_INVALID;
        SCH_tasks_G[i].prev   = IDX_INVALID;
        SCH_tasks_G[i].in_use = 0;
    }

    Error_code_G = 0;

}

void SCH_Update(void) {
    if (g_delta_head != IDX_INVALID) {
        uint8_t idx = g_delta_head;
        if (SCH_tasks_G[idx].Delay > 0) {
            SCH_tasks_G[idx].Delay--;
        }
        if (SCH_tasks_G[idx].Delay == 0) {
            uint8_t next = SCH_tasks_G[idx].next;
            if (next != IDX_INVALID) {
                SCH_tasks_G[next].prev = IDX_INVALID;
            }
            g_delta_head = next;
            SCH_tasks_G[idx].next = SCH_tasks_G[idx].prev = IDX_INVALID;

            SCH_tasks_G[idx].RunMe++;
            ready_push(idx);

        }
    }

    Watchdog_Refresh();
    SCH_Report_Status();
}

static const char* SCH_ErrorString(unsigned char code) {
    switch (code) {
        case 0: return "[OK] No error\r\n";
        case ERROR_SCH_TOO_MANY_TASKS:
            return "[ERROR] Too many tasks scheduled\r\n";
        case ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK:
            return "[ERROR] Waiting for slave to ACK\r\n";
        case ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER:
            return "[ERROR] Waiting for start command from master\r\n";
        case ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START:
            return "[ERROR] One or more slaves did not start\r\n";
        case ERROR_SCH_LOST_SLAVE:
            return "[ERROR] Lost slave connection\r\n";
        case ERROR_SCH_CAN_BUS_ERROR:
            return "[ERROR] CAN bus error\r\n";
        case ERROR_I2C_WRITE_BYTE_AT24C64:
            return "[ERROR] I2C write error (AT24C64)\r\n";
        default:
            return "[ERROR] Unknown error code\r\n";
    }
}

void SCH_Report_Status(void) {
#ifdef SCH_REPORT_ERRORS
    if (Error_code_G != Last_error_code_G) {
        UART_Print(SCH_ErrorString(Error_code_G));

        Last_error_code_G  = Error_code_G;
        Error_tick_count_G = (Error_code_G != 0) ? ERROR_HOLD_TICKS : 0;
    } else {
        if (Error_tick_count_G != 0) {
            if (--Error_tick_count_G == 0) {
                Error_code_G = 0;
                UART_Print("[INFO] Error cleared\r\n");
            }
        }
    }
#endif
}

unsigned char SCH_Add_Task(void (*pFunction)(), unsigned int DELAY, unsigned int PERIOD) {
    uint8_t idx = list_pop_free();
    if (idx == IDX_INVALID) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        return SCH_MAX_TASKS; // no slot
    }

    SCH_tasks_G[idx].pTask  = pFunction;
    SCH_tasks_G[idx].Period = PERIOD;
    SCH_tasks_G[idx].RunMe  = 0;

    // Insert into delta list by requested start delay
    delta_insert_by_delay(idx, DELAY);
    return idx;
}

unsigned char SCH_Delete_Task(const unsigned char TASK_INDEX) {
    uint8_t idx = TASK_INDEX;
    if (idx >= SCH_MAX_TASKS || !SCH_tasks_G[idx].in_use) return 0;
    if (SCH_tasks_G[idx].prev != IDX_INVALID || idx == g_delta_head) {
        delta_remove(idx);
    }
    SCH_tasks_G[idx].pTask = 0;
    list_push_free(idx);
    return 1;
}

void SCH_Dispatch_Tasks(void) {
    while (g_ready_head != IDX_INVALID) {
        uint8_t idx = g_ready_head;
        g_ready_head = SCH_tasks_G[idx].next;
        SCH_tasks_G[idx].next = IDX_INVALID;

        if (SCH_tasks_G[idx].RunMe) {
            SCH_tasks_G[idx].RunMe--;
            void (*fn)(void) = SCH_tasks_G[idx].pTask;
            uint32_t period = SCH_tasks_G[idx].Period;

            if (fn) fn();

            if (period > 0) {
                delta_insert_by_delay(idx, period);
            } else {
                SCH_tasks_G[idx].pTask = 0;
                list_push_free(idx);
            }
        }
    }

    SCH_Go_To_Sleep();
}

void SCH_Go_To_Sleep(void) {
	__WFI();
}
