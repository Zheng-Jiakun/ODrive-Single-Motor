#include "user_main.h"

// float motor_current = 0.1f, motor_speed = 0, motor_position = 0;

uint8_t braking_flag = 0;

void user_setup()
{
    // pid_init(&motor_pid_current, 5, 50, 50, 50.0f, 5.0f, 0.5f);
    // pid_init(&motor_pid_speed, -40, 40, 50, 0.05f, 0.005f, 0.001f);
    // pid_init(&motor_pid_position, -10, 10, 10, 1.0f, 0.00f, 0.0f);

    user_control_init();
    while (wait_for_system_start())
        ;
    can_init();
    M3508_init();
    HAL_TIM_Base_Start_IT(&htim7);
    while (calibrate_flag != 3);
    LED_init();
    MPU6050_init();
    motor.pwm = 0;
    motor_start();
    // HAL_TIM_Base_Start_IT(&htim6);

    // HAL_TIM_Base_Start_IT(&htim10);
}

void user_loop()
{
    // HAL_Delay(3000);
    // HAL_NVIC_SystemReset();
    if (check_system_halt() == 0)
    {
        LED_off();
        HAL_Delay(100);
        HAL_NVIC_SystemReset();
    }

    user_update_throttle();
    MPU6050_update_data();
    LED_control();
    HAL_Delay(1);
}