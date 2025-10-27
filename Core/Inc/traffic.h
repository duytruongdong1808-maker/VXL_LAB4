#ifndef __TRAFFIC_H__
#define __TRAFFIC_H__

#include "main.h"

// Enum trạng thái đèn giao thông
enum TrafficState {
    S1_GREEN_RED,
    S2_AMBER_RED,
    S3_RED_GREEN,
    S4_RED_AMBER
};

// Thời gian mỗi pha
extern int red_time;
extern int amber_time;
extern int green_time;

// Biến đếm thời gian 2 đường
extern int counter_road1;
extern int counter_road2;

// Prototype các hàm trong traffic.c
void traffic_init(void);
void traffic_run(void);
void traffic_clear_all_leds(void);

#endif /* __TRAFFIC_H__ */
