#include "timer.h"
#include "scheduler.h"

extern TIM_HandleTypeDef htim2;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        SCH_Update();
    }
}
