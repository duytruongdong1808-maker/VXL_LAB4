#include "input_reading.h"
#include "display.h"
#include "traffic.h"
#include "main.h"

int mode = 1;
int prev_mode = 1;
int temp_red_time = 0;
int temp_amber_time = 0;
int temp_green_time = 0;

void fsm_traffic_run(void)
{
    if (is_button_pressed(0))
    {
        mode++;
        if (mode >= 5)
            mode = 1;
    }

    if (mode != prev_mode)
	{
		traffic_clear_all_leds();

		if (mode == 1) {
			traffic_init();
		} else if (mode == 2) {
			temp_red_time = red_time;
			led_buffer[0] = 0;
			led_buffer[1] = mode;
			led_buffer[2] = temp_red_time / 10;
			led_buffer[3] = temp_red_time % 10;
		}  else if (mode == 3) {
			temp_amber_time = amber_time;
			led_buffer[0] = 0;
			led_buffer[1] = mode;
			led_buffer[2] = temp_amber_time / 10;
			led_buffer[3] = temp_amber_time % 10;
		} else if (mode == 4) {
            temp_green_time = green_time;
            led_buffer[0] = 0;
            led_buffer[1] = mode;
            led_buffer[2] = temp_green_time / 10;
            led_buffer[3] = temp_green_time % 10;
        }
		prev_mode = mode;

	}

    switch(mode)
    {
        case 1:
        	if (red_time != amber_time + green_time) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);
        	else traffic_run();
            break;
        case 2:
        	if (is_button_pressed_1s(1)) temp_red_time = 1;
			else if (is_button_pressed(1)) {
				temp_red_time++;
				if (temp_red_time > 99) temp_red_time = 1;
			}

			led_buffer[2] = temp_red_time / 10;
			led_buffer[3] = temp_red_time % 10;

			if (is_button_pressed(2)) {
				red_time = temp_red_time;
				if (green_time < 1) green_time = 1;
			}
			break;
        case 3:
        	if (is_button_pressed_1s(1)) temp_amber_time = 1;
			else if (is_button_pressed(1)) {
				temp_amber_time++;
				if (temp_amber_time > 99) temp_amber_time = 1;
			}

			led_buffer[2] = temp_amber_time / 10;
			led_buffer[3] = temp_amber_time % 10;

			if (is_button_pressed(2)) {
				amber_time = temp_amber_time;
				if (green_time < 1) green_time = 1;
			}
            break;
        case 4:
        	if (is_button_pressed_1s(1)) temp_green_time = 1;
			else if (is_button_pressed(1)) {
				temp_green_time++;
				if (temp_green_time > 99) temp_green_time = 1;
			}

			led_buffer[2] = temp_green_time / 10;
			led_buffer[3] = temp_green_time % 10;

			if (is_button_pressed(2)) {
				green_time = temp_green_time;
			}
			break;
        default:
            mode = 1;
            break;
    }
}
