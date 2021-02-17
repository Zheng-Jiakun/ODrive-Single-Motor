#ifndef THROTTLE_H
#define THROTTLE_H

#define THROTTLE_NUM     2
#define LEFT_THROTTLE    0
#define RIGHT_THROTTLE   1

#define TURN_ANGLE       8

#define SPEED_UP_LIMIT   100
#define THROTTLE_HOLD_TIME  1000

#define THROTTLE_ON         2000

#include "main.h"
#include "adc.h"
#include "motor_driver.h"

extern uint16_t throttle[THROTTLE_NUM];

void  user_control_init ();
uint8_t wait_for_system_start();
void user_update_throttle ();
uint8_t check_system_halt();
void LED_control();

#endif