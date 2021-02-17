#ifndef USER_MAIN
#define USER_MAIN

#include "main.h"
#include "can_protocol.h"
#include "motor_driver.h"
#include "algorithm.h"
#include "tim.h"
#include "can_protocol.h"
#include "LED.h"
#include "M3508.h"
#include "user_control.h"


extern uint8_t braking_flag;
extern float motor_current, motor_speed, motor_position;

void user_setup();
void user_loop();

#endif