#include "traffic.h"
#include "display.h"
#include "main.h"

int red_time   = 10;
int amber_time = 3;
int green_time = 7;

int counter_road1 = 0;
int counter_road2 = 0;

enum TrafficState state = S1_GREEN_RED;

void traffic_clear_all_leds(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                             GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
}

static inline void traffic_set_led_buffer(void){
    led_buffer[0] = counter_road1 / 10;
    led_buffer[1] = counter_road1 % 10;
    led_buffer[2] = counter_road2 / 10;
    led_buffer[3] = counter_road2 % 10;
}

void traffic_init(void)
{
	state = S1_GREEN_RED;
	counter_road1 = green_time;
	counter_road2 = red_time;
	traffic_set_led_buffer();
	traffic_clear_all_leds();
}

void traffic_run(void)
{
	switch (state)
	{
	case S1_GREEN_RED:
		traffic_clear_all_leds();
		traffic_set_led_buffer();

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

		if (counter_road1 <= 0)
		{
			state = S2_AMBER_RED;
			counter_road1 = amber_time;
		}
		break;

	case S2_AMBER_RED:
		traffic_clear_all_leds();
		traffic_set_led_buffer();

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

		if (counter_road1 <= 0)
		{
			state = S3_RED_GREEN;
			counter_road1 = red_time;
			counter_road2 = green_time;
		}
		break;

	case S3_RED_GREEN:
		traffic_clear_all_leds();
		traffic_set_led_buffer();

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

		if (counter_road2 <= 0)
		{
			state = S4_RED_AMBER;
			counter_road2 = amber_time;
		}
		break;

	case S4_RED_AMBER:
		traffic_clear_all_leds();
		traffic_set_led_buffer();

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

		if (counter_road2 <= 0)
		{
			state = S1_GREEN_RED;
			counter_road1 = green_time;
			counter_road2 = red_time;
		}
		break;
	}
}

