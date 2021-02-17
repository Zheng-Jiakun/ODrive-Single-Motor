#ifndef MOTOR_DRIVER
#define MOTOR_DRIVER

#define PWM_DUTYCYCLE2PULSE(x)      ((float)(x)/100.0f*63)

#include "main.h"
#include "tim.h"
#include "spi.h"
#include "adc.h"
#include "algorithm.h"
#include "user_main.h"
#include <stdlib.h>

#define ADC_MEANFILTER_SIZE         100
#define SPEED_FILTER_SIZE           240
#define ZERO_SPEED_TIMEOUT          10000U

#define DRV8301_MODE_W              0
#define DRV8301_MODE_R              1
#define DRV8301_ADDRESS_STATUS0     0x00
#define DRV8301_ADDRESS_STATUS1     0x01
#define DRV8301_ADDRESS_CONTROL1    0x02
#define DRV8301_ADDRESS_CONTROL2    0x03

#define DRV8301_RW_BIT_OFFSET       15
#define DRV8301_ADDRESS_BIT_OFFSET  11

#define DRV8301_GET_SPI_FRAME(rw, address, data)   ((rw) << DRV8301_RW_BIT_OFFSET) | ((address) << DRV8301_ADDRESS_BIT_OFFSET | (data))

#define DRV8301_CURRENT_AMP_GAIN    80.0f
#define CURRENT_SAMPLE_RES          0.002f
#define ADC2VOLTAGE(x)              ((x)/4095.0f*3.3f)
#define CURRENT_LINEAR_OFFSET       -0.01f
#define DRV8301_VOLTAGE2CURRENT(x)  ((1.65f-(x))/DRV8301_CURRENT_AMP_GAIN/(CURRENT_SAMPLE_RES)+CURRENT_LINEAR_OFFSET)
// #define DRV8301_VOLTAGE2CURRENT(x)  (-10.068f*(x)+16.037+CURRENT_LINEAR_OFFSET)
// #define DRV8301_VOLTAGE2CURRENT(x)  (-5.2726f*(x)+8.4632+CURRENT_LINEAR_OFFSET)

#define POLAR_NUM                   60
#define HALL2DEGREE(x)              (360.0f/(float)POLAR_NUM*(x))
#define DEGREE2HALL(x)              ((x)*(float)POLAR_NUM/360.0f)

typedef enum {
    HIGH_ON_LOW_OFF = 0,
    HIGH_OFF_LOW_ON,
    HIGH_OFF_LOW_OFF
} mosfet_state_t;

typedef struct {
    int8_t pwm;
    int8_t pwm_buffer;
    int16_t rpm;
    float current;
    int32_t position;
    float degree;
} motor_t;

extern pid_t motor_pid_current, motor_pid_speed, motor_pid_position;
extern motor_t motor;

void mosfet_control(uint8_t n, mosfet_state_t s);
void motor_start();
void motor_change_phase();
void motor_get_speed();
void motor_check_0_speed();
void motor_get_current();
void motor_get_position();
void motor_current_loop(float set);
void motor_speed_loop(float set);
void motor_position_loop(float set);
void adc_filter();
void motor_pwm_buffer();

#endif