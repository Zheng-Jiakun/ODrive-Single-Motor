#include "MPU6050.h"
#include "math.h"

struct ACCELSTRUCT accelStruct = {0, 0, 0};
struct GYROSTRUCT gyroStruct = {0, 0, 0};

ANGLESTRUCT accAngle, gyroAngle, filteredAngle;

#define GYRO_LSB 131.0f
#define ACCEL_LSB 16384.0f
#define G_value 9.8f

void MPU6050_init(void)
{
    iic_init();

    MPU6050_write_byte(MPU6050_RA_PWR_MGMT_1, 0x01); // exit sleeping mode, x axis of gyro as sampling clock
    MPU6050_write_byte(MPU6050_RA_SMPLRT_DIV, 0x04);     // clock dividor 4, 1k/4，sampling rate 25Hz。
    MPU6050_write_byte(MPU6050_RA_CONFIG, 2); // LPF, 100Hz cut off
    MPU6050_write_byte(MPU6050_RA_GYRO_CONFIG, 3 << 3);  // gyrosocpe range, 250dps
    MPU6050_write_byte(MPU6050_RA_ACCEL_CONFIG, 2 << 3); // accerometer range，2g。
    MPU6050_write_byte(MPU6050_RA_INT_PIN_CFG, 0x32); // high level interrupt signal, push-pull output, wait until reading
    MPU6050_write_byte(MPU6050_RA_INT_ENABLE, 0x01);  // use "data ready" interrupt
    MPU6050_write_byte(MPU6050_RA_USER_CTRL, 0x00);
}

void MPU6050_write_byte(uint8_t add, uint8_t val)
{
    iic_send_start();

    iic_send_byte(MPU6050_ADDRESS_AD0_LOW);
    iic_wait_ack();

    iic_send_byte(add);
    iic_wait_ack();

    iic_send_byte(val);
    iic_wait_ack();

    iic_send_stop();

    // HAL_Delay(1);
}

void MPU6050_read_byte(uint8_t add, uint8_t *val)
{
    iic_send_start();

    iic_send_byte(MPU6050_ADDRESS_AD0_LOW);
    iic_wait_ack();

    iic_send_byte(add);
    iic_wait_ack();

    iic_send_start();

    iic_send_byte(MPU6050_ADDRESS_AD0_LOW + 1);
    iic_wait_ack();

    iic_read_byte(val);
    iic_send_nack();

    iic_send_stop();
}

void MPU6050_update_data(void)
{
    uint8_t data_H, data_L;
    MPU6050_read_byte(MPU6050_RA_ACCEL_XOUT_H, &data_H);
    MPU6050_read_byte(MPU6050_RA_ACCEL_XOUT_L, &data_L);
    accelStruct.accelZ = (int16_t)(data_H << 8 | data_L);

    MPU6050_read_byte(MPU6050_RA_ACCEL_YOUT_H, &data_H);
    MPU6050_read_byte(MPU6050_RA_ACCEL_YOUT_L, &data_L);
    accelStruct.accelY = (int16_t)(data_H << 8 | data_L);

    MPU6050_read_byte(MPU6050_RA_ACCEL_ZOUT_H, &data_H);
    MPU6050_read_byte(MPU6050_RA_ACCEL_ZOUT_L, &data_L);
    accelStruct.accelX = (int16_t)(data_H << 8 | data_L);

    MPU6050_read_byte(MPU6050_RA_GYRO_XOUT_H, &data_H);
    MPU6050_read_byte(MPU6050_RA_GYRO_XOUT_L, &data_L);
    gyroStruct.gyroX = (int16_t)(data_H << 8 | data_L);// + 1.40f;

    MPU6050_read_byte(MPU6050_RA_GYRO_YOUT_H, &data_H);
    MPU6050_read_byte(MPU6050_RA_GYRO_YOUT_L, &data_L);
    gyroStruct.gyroY = (int16_t)(data_H << 8 | data_L);

    MPU6050_read_byte(MPU6050_RA_GYRO_ZOUT_H, &data_H);
    MPU6050_read_byte(MPU6050_RA_GYRO_ZOUT_L, &data_L);
    gyroStruct.gyroZ = (int16_t)(data_H << 8 | data_L);// + 2.20f;

    // static uint32_t preivous_tick;
    // static float elapsed_tick;
    // elapsed_tick = (HAL_GetTick() - preivous_tick) / 1000.0f;
    // preivous_tick = HAL_GetTick();

    // gyroAngle.angleX += gyroStruct.gyroX * elapsed_tick;
    // gyroAngle.angleY += gyroStruct.gyroY * elapsed_tick;

    // static float x_temp = 0.0f, y_temp = 0.0f;
    // static uint8_t filter_cnt = 0;
    // x_temp += (atan(accelStruct.accelY / sqrt(pow(accelStruct.accelX, 2) + pow(accelStruct.accelZ, 2))) * 180 / M_PI);
    // y_temp += (atan(-accelStruct.accelX / sqrt(pow(accelStruct.accelY, 2) + pow(accelStruct.accelZ, 2))) * 180 / M_PI);

    // if (filter_cnt >= MEAN_FILTER_SIZE)
    // {
    //     accAngle.angleX = x_temp / (float)MEAN_FILTER_SIZE;
    //     accAngle.angleY = y_temp / (float)MEAN_FILTER_SIZE;
    //     x_temp = 0;
    //     y_temp = 0;
    //     filter_cnt = 0;
    // }
    // filter_cnt++;

    accAngle.angleX = (atan(accelStruct.accelY / sqrt(pow(accelStruct.accelX, 2) + pow(accelStruct.accelZ, 2))) * 180 / M_PI);
    accAngle.angleY = (atan(-accelStruct.accelX / sqrt(pow(accelStruct.accelY, 2) + pow(accelStruct.accelZ, 2))) * 180 / M_PI);

    // filteredAngle.angleX = gyroAngle.angleX * 0.96f + accAngle.angleX * 0.04f;
    // filteredAngle.angleY = gyroAngle.angleY * 0.96f + accAngle.angleY * 0.04f;
}
