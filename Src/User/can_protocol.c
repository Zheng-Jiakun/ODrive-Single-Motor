#include "can_protocol.h"
#include <string.h>

HAL_StatusTypeDef CanRxState, CanTxState;

CAN_RxHeaderTypeDef CanRxHeader;
uint8_t CanRxData[8];

CAN_TxHeaderTypeDef CanTxHeader;
uint8_t CanTxData[8];
uint32_t TxMailBox = CAN_TX_MAILBOX0;

MotorDataTypeDef motor_data[4];

void can_init()
{
    CAN_FilterTypeDef Canfilter;
    Canfilter.FilterIdHigh = 0x201 << 5;
    Canfilter.FilterIdLow = 0x202 << 5;
    Canfilter.FilterMaskIdHigh = 0x203 << 5;
    Canfilter.FilterMaskIdLow = 0x204 << 5;
    Canfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    Canfilter.FilterBank = 0;
    Canfilter.FilterMode = CAN_FILTERMODE_IDLIST;
    Canfilter.FilterScale = CAN_FILTERSCALE_16BIT;
    Canfilter.FilterActivation = CAN_FILTER_ENABLE;
    Canfilter.SlaveStartFilterBank = 0; //meaningless for STM32F103C8

    if (HAL_CAN_ConfigFilter(&hcan1, &Canfilter) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_CAN_Start(&hcan1);

    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void update_speed()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        CanTxData[2 * i] = motor_data[i].target_current >> 8;
        CanTxData[2 * i + 1] = motor_data[i].target_current;
    }
}

void encoder_data_handler()
{
    if (motor_data[0].position - motor_data[0].last_position > 5000)
        motor_data[0].round_count--;
    if (motor_data[0].position - motor_data[0].last_position < -5000)
        motor_data[0].round_count++;

    motor_data[0].accumulated_position = motor_data[0].round_count * 8192 + motor_data[0].position;

    motor_data[0].last_position = motor_data[0].position;
}

void feedback_decode()
{
    uint8_t motor_id = CanRxHeader.StdId - 0x200 - 1;
    motor_data[motor_id].position = CanRxData[0] << 8 | CanRxData[1];
    motor_data[motor_id].speed = CanRxData[2] << 8 | CanRxData[3];
    motor_data[motor_id].torque = CanRxData[4] << 8 | CanRxData[5];
    motor_data[motor_id].temperature = CanRxData[6] << 8 | CanRxData[7];

    encoder_data_handler();
}

void can_rx_data()
{
    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0)
    {
        CanRxState = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CanRxHeader, CanRxData);
    }
    feedback_decode();
}

void can_tx_data()
{
    update_speed();

    // uint32_t start_tick = HAL_GetTick();
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0)
    //     if (HAL_GetTick() - start_tick > 3000)
    //         return;

    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 0)
    {
        CanTxHeader.StdId = 0x200;
        // CanTxHeader.ExtId = 0x0000;
        CanTxHeader.IDE = CAN_ID_STD;
        CanTxHeader.RTR = CAN_RTR_DATA;
        CanTxHeader.DLC = 8;
        CanTxState = HAL_CAN_AddTxMessage(&hcan1, &CanTxHeader, CanTxData, &TxMailBox);
    }
}
