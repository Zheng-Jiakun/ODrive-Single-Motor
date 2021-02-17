#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
#include "can_protocol.h"
#include "MPU6050.h"

#define IMU_OFF_SET         -0.0f
#define IMU_RANGE           60.0f

#define ABS(x)               (x >= 0? x : -x)

#define CALIBRATE_SPEED      1000
#define STUCK_TORQUE         4000
#define STUCK_SPEED          100
#define STUCK_TIMEOUT        10
#define CALIBRATED_ANGLE     60

#define M3508_MAX_OUTPUT        500
#define M3508_POSITION_LIMIT    3000

#define STUCK_CONDITION      (ABS(motor_data[0].torque) > STUCK_TORQUE)// && ABS(motor_data[0].speed) < STUCK_SPEED)
#define NOT_STUCK_CONDITION  (ABS(motor_data[0].speed) > STUCK_SPEED)

#define ANGLE2ENCODER(x)    (x/360.0f*8192.0f*36.0f)
#define ENCODER2ANGLE(x)    (x*360.0f/8192.0f/36.0f)

void M3508_init();
void M3508_update_control();
void motor_calibrate ();

void set_motor_angle(float a);

extern float angle_range;
extern uint8_t calibrate_flag;

#endif