#include "M3508.h"
#include "algorithm.h"

// #define DEBUG

pid_t M3508_speed_pid, M3508_position_pid;
int32_t M3508_position;
int16_t M3508_speed = 0;

uint8_t calibrate_flag = 0;
int32_t left_position, right_position;

float angle_range = 0.0f;

void M3508_init()
{
    pid_init(&M3508_speed_pid, -10000.0f, 10000.0f, 10000.f, 5.5f, 0.1f, 0.0f);
    pid_init(&M3508_position_pid, -M3508_MAX_OUTPUT, M3508_MAX_OUTPUT, M3508_MAX_OUTPUT, 0.05f, 0.0f, 0.0f);
}

void set_motor_speed(int16_t speed)
{
    M3508_speed = speed;
}

void M3508_update_control()
{
    if (calibrate_flag == 0)
    {
        set_motor_speed(CALIBRATE_SPEED);
        static uint32_t stuck_tick;
        static uint8_t stuck_flag = 0;
        if (STUCK_CONDITION && stuck_flag == 0)
        {
            stuck_tick = HAL_GetTick();
            stuck_flag = 1;
        }
        else if (NOT_STUCK_CONDITION)
        {
            stuck_tick = 0;
            stuck_flag = 0;
        }
        else if (stuck_flag == 1 && HAL_GetTick() - stuck_tick > STUCK_TIMEOUT)
        {
            left_position = motor_data[0].accumulated_position;
            calibrate_flag = 1;
            stuck_flag = 0;
            stuck_tick = 0;
        }
    }
    else if (calibrate_flag == 1)
    {
        set_motor_speed(-CALIBRATE_SPEED);
        static uint32_t stuck_tick;
        static uint8_t stuck_flag = 0;
        if (STUCK_CONDITION && stuck_flag == 0)
        {
            stuck_tick = HAL_GetTick();
            stuck_flag = 1;
        }
        else if (NOT_STUCK_CONDITION)
        {
            stuck_tick = 0;
            stuck_flag = 0;
        }
        else if (stuck_flag == 1 && HAL_GetTick() - stuck_tick > STUCK_TIMEOUT)
        {
            right_position = motor_data[0].accumulated_position;
            calibrate_flag = 2;
            stuck_flag = 0;
            stuck_tick = 0;
        }
    }
    else if (calibrate_flag == 2)
    {
        // set_motor_speed(0);
        if (ABS(left_position - right_position) > ANGLE2ENCODER(CALIBRATED_ANGLE))
        {
            calibrate_flag = 3;
            // angle_range = ABS(ENCODER2ANGLE((right_position - left_position) / 2));
        }
        else
        {
            calibrate_flag = 0;
            left_position = 0;
            right_position = 0;
        }
    }
    else if (calibrate_flag == 3)
    {
#ifdef DEBUG
        // M3508_position = (left_position + right_position) / 2;
        set_motor_angle((throttle[LEFT_THROTTLE] - 1100) * ABS(right_position - left_position) / 2100);
#else
        set_motor_angle((accAngle.angleY + IMU_OFF_SET) / IMU_RANGE * ABS((right_position - left_position) / 2));
#endif
        M3508_speed = pid_calculate(&M3508_position_pid, motor_data[0].accumulated_position, M3508_position);
    }
    motor_data[0].target_current = pid_calculate(&M3508_speed_pid, motor_data[0].speed, M3508_speed);
    can_tx_data();
}

void set_motor_angle(float a)
{
    int32_t temp_position = (left_position + right_position) / 2 + a;
    if (temp_position < right_position + M3508_POSITION_LIMIT)
        M3508_position = right_position + M3508_POSITION_LIMIT;
    else if (temp_position > left_position - M3508_POSITION_LIMIT)
        M3508_position = left_position - M3508_POSITION_LIMIT;
    else
        M3508_position = temp_position;
}