#ifndef CAN_PROTOCOL
#define CAN_PROTOCOL

#include "main.h"
#include "can.h"
#include "tim.h"
#include "motor_driver.h"

void can_init();
void can_rx_data();
void can_tx_data();


#define MOTOR_ID_1          0x200
#define MOTOR_ID_2          0x1ff
#define MOTOR_FEEDBACK_ID   0x200

typedef struct 
{
    int16_t target_current;
    int32_t accumulated_position;

    uint16_t position;
    int16_t speed;
    int16_t current;
    int16_t torque;
    uint8_t temperature; 

    int16_t round_count;
    uint16_t last_position;
} MotorDataTypeDef;

extern MotorDataTypeDef motor_data[4];

#endif