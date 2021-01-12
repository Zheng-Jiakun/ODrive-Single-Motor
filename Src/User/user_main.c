#include "user_main.h"
#include "motor_driver.h"
#include "algorithm.h"
#include "tim.h"

pid_t motor_pid;

float motor_current = 0.1f, motor_speed = 500, motor_position = 0;

void user_setup()
{
    // pid_init(&motor_pid_current, 5, 50, 50, 50.0f, 5.0f, 0.5f);
    // pid_init(&motor_pid_speed, -40, 40, 50, 0.05f, 0.005f, 0.001f);
    // pid_init(&motor_pid_position, -10, 10, 10, 1.0f, 0.00f, 0.0f);

    motor.pwm = 30;
    motor_start();

    HAL_TIM_Base_Start_IT(&htim7);
}

void user_loop()
{
    // motor_current_loop(motor_current);
    // motor_speed_loop(motor_speed);
    // motor_position_loop(motor_position);

    // motor_speed = pid_calc(&motor_pid_position, motor.position, motor_position);
    // motor_current = pid_calc(&motor_pid_speed, motor.current, motor_speed);
    // motor.pwm = motor_current;
    HAL_Delay(1);
    // motor_position += DEGREE2HALL(90);
}