#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "main.h"

extern int led_buffer[4];
extern int index_led;

void display7SEG(uint8_t num);

void update7SEG(int index);

void display_mode_on_7seg(void);

void blink_leds_by_mode(void);

#endif /* __DISPLAY_H__ */
