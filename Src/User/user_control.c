#include "user_control.h"

uint16_t throttle[THROTTLE_NUM] = {};

uint8_t wait_for_system_start()
{
    static uint8_t triggered_flag = 0;
    static uint32_t triggered_tick = 0;
    if (throttle[LEFT_THROTTLE] > THROTTLE_ON && throttle[RIGHT_THROTTLE] > THROTTLE_ON)
    {
        if (triggered_flag == 0)
        {
            triggered_tick = HAL_GetTick();
            triggered_flag = 1;
        }
        else if (triggered_flag == 1 && HAL_GetTick() - triggered_tick > THROTTLE_HOLD_TIME)
        {
            HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
            HAL_Delay(500);
            HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
            // HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
            return 0;
        }
    }
    else //if (throttle[LEFT_THROTTLE] <= THROTTLE_ON || throttle[RIGHT_THROTTLE] <= THROTTLE_ON)
    {
        triggered_flag = 0;
        return 1;
    }
    return 1;
}

uint8_t check_system_halt()
{
    static uint8_t triggered_flag = 0;
    static uint32_t triggered_tick = 0;
    if (throttle[LEFT_THROTTLE] > THROTTLE_ON && throttle[RIGHT_THROTTLE] > THROTTLE_ON)
    {
        if (triggered_flag == 0)
        {
            triggered_tick = HAL_GetTick();
            triggered_flag = 1;
        }
        else if (triggered_flag == 1 && HAL_GetTick() - triggered_tick > THROTTLE_HOLD_TIME)
        {
            HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
            HAL_Delay(500);
            HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
            // HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
            return 0;
        }
    }
    else //if (throttle[LEFT_THROTTLE] <= THROTTLE_ON || throttle[RIGHT_THROTTLE] <= THROTTLE_ON)
    {
        triggered_flag = 0;
        return 1;
    }
    return 1;
}

void user_control_init()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)throttle, THROTTLE_NUM);
}

void user_update_throttle()
{
    int8_t left = 0, right = 0;
    left = (throttle[LEFT_THROTTLE] - 1100) * SPEED_UP_LIMIT / 2100;
    if (left < 0)
        left = 0;
    else if (left > SPEED_UP_LIMIT)
        left = SPEED_UP_LIMIT;

    right = (throttle[RIGHT_THROTTLE] - 1100) * SPEED_UP_LIMIT / 2100;
    if (right < 0)
        right = 0;
    else if (right > SPEED_UP_LIMIT)
        right = SPEED_UP_LIMIT;

    if (right < 95)
    {
        if (left - right > 0)
            motor.pwm = -(left - right);
        else
            motor.pwm = 0;
        braking_flag = 0;
    }
    else
        braking_flag = 1;
}

void LED_control()
{
    if (braking_flag)
    {
        motor.pwm = 0;
        LED_stop();
    }
    else if (accAngle.angleY < -TURN_ANGLE)
    {
        LED_left();
    }
    else if (accAngle.angleY > TURN_ANGLE)
    {
        LED_right();
    }
    else
    {
        LED_go();
    }
}