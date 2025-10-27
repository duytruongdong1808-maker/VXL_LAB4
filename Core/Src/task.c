#include "main.h"
#include "task.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;

void UART_Print(const char *msg) {
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void Task_PrintTimestamp(void) {
    static uint32_t ms = 0;
    char buf[64];

    ms += 10;
    sprintf(buf, "Current timestamp: %lu ms\r\n", ms);
    UART_Print(buf);
}

void Task1(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
    UART_Print("Task1 triggered\r\n");
}

// Task2 – 1.0s: Toggle LED PB8
void Task2(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
    UART_Print("Task2 triggered\r\n");
}

// Task3 – 1.5s: Toggle LED PB9
void Task3(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
    UART_Print("Task3 triggered\r\n");
}

// Task4 – 2.0s: Toggle LED PB10
void Task4(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
    UART_Print("Task4 triggered\r\n");
}

// Task5 – 2.5s: Toggle LED PB11
void Task5(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
    UART_Print("Task5 triggered\r\n");
}

void LED_OneShot(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
    UART_Print("One shot task triggered\r\n");
}

