#include "motor_driver.h"

motor_t motor;

uint16_t drv8301_spi_rx_dat, drv8301_spi_tx_dat;
uint16_t adc_result_raw, adc_result;
float current_dc_offset;

pid_t motor_pid_current, motor_pid_speed, motor_pid_position;

const uint8_t hall_state_sequence[] = {0, 6, 4, 5, 2, 1, 3, 0}; //map hall states as 1~6 or 6~1

// {0, 6, 4, 5, 2, 1, 3, 0}{0, 1, 3, 2, 5, 6, 4, 0}

uint8_t read_hall()
{
    uint8_t hall = 0b000;

    if (HAL_GPIO_ReadPin(M1_HALL_A_GPIO_Port, M1_HALL_A_Pin) == GPIO_PIN_SET)
        hall |= 0b001;
    if (HAL_GPIO_ReadPin(M1_HALL_B_GPIO_Port, M1_HALL_B_Pin) == GPIO_PIN_SET)
        hall |= 0b100;
    if (HAL_GPIO_ReadPin(M1_HALL_C_GPIO_Port, M1_HALL_C_Pin) == GPIO_PIN_SET)
        hall |= 0b010;

    return hall;
}

void mosfet_control(uint8_t n, mosfet_state_t s)
{
    switch (n)
    {
    case 1:
        if (s == HIGH_OFF_LOW_ON)
        {
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PWM_DUTYCYCLE2PULSE(0));
        }
        else if (s == HIGH_ON_LOW_OFF)
        {
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PWM_DUTYCYCLE2PULSE(abs(motor.pwm_buffer)));
        }
        else if (s == HIGH_OFF_LOW_OFF)
        {
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
        }
        break;

    case 2:
        if (s == HIGH_OFF_LOW_ON)
        {
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PWM_DUTYCYCLE2PULSE(0));
        }
        else if (s == HIGH_ON_LOW_OFF)
        {
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PWM_DUTYCYCLE2PULSE(abs(motor.pwm_buffer)));
        }
        else if (s == HIGH_OFF_LOW_OFF)
        {
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
        }
        break;

    case 3:
        if (s == HIGH_OFF_LOW_ON)
        {
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PWM_DUTYCYCLE2PULSE(0));
        }
        else if (s == HIGH_ON_LOW_OFF)
        {
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PWM_DUTYCYCLE2PULSE(abs(motor.pwm_buffer)));
        }
        else if (s == HIGH_OFF_LOW_OFF)
        {
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
        }
        break;

    default:
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
        break;
    }
}

void phase_select(uint8_t phase)
{
    switch (phase)
    {
    case 0:
        mosfet_control(1, HIGH_OFF_LOW_OFF);
        mosfet_control(2, HIGH_OFF_LOW_OFF);
        mosfet_control(3, HIGH_OFF_LOW_OFF);
        break;

    case 1: //AB
        mosfet_control(1, HIGH_ON_LOW_OFF);
        mosfet_control(2, HIGH_OFF_LOW_ON);
        mosfet_control(3, HIGH_OFF_LOW_OFF);
        break;

    case 2: //CB
        mosfet_control(1, HIGH_OFF_LOW_OFF);
        mosfet_control(2, HIGH_OFF_LOW_ON);
        mosfet_control(3, HIGH_ON_LOW_OFF);
        break;

    case 3: //CA
        mosfet_control(1, HIGH_OFF_LOW_ON);
        mosfet_control(2, HIGH_OFF_LOW_OFF);
        mosfet_control(3, HIGH_ON_LOW_OFF);
        break;

    case 4: //BA
        mosfet_control(1, HIGH_OFF_LOW_ON);
        mosfet_control(2, HIGH_ON_LOW_OFF);
        mosfet_control(3, HIGH_OFF_LOW_OFF);
        break;

    case 5: //BC
        mosfet_control(1, HIGH_OFF_LOW_OFF);
        mosfet_control(2, HIGH_ON_LOW_OFF);
        mosfet_control(3, HIGH_OFF_LOW_ON);
        break;

    case 6: //AC
        mosfet_control(1, HIGH_ON_LOW_OFF);
        mosfet_control(2, HIGH_OFF_LOW_OFF);
        mosfet_control(3, HIGH_OFF_LOW_ON);
        break;

    default:
        break;
    }
}

void drv8301_spi_sent()
{
    HAL_GPIO_WritePin(M1_SPI_CS_GPIO_Port, M1_SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi3, (uint8_t *)&drv8301_spi_tx_dat, 1, 100);
    HAL_Delay(1);
    HAL_GPIO_WritePin(M1_SPI_CS_GPIO_Port, M1_SPI_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(M1_SPI_CS_GPIO_Port, M1_SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_SPI_Receive(&hspi3, (uint8_t *)&drv8301_spi_rx_dat, 1, 100);
    HAL_Delay(1);
    HAL_GPIO_WritePin(M1_SPI_CS_GPIO_Port, M1_SPI_CS_Pin, GPIO_PIN_SET);
}

void motor_start()
{
    // while (read_hall() == 0b000 || read_hall() == 0b111)
    //     ;

    /*-----------------------------------------------------------------------------------------------------------------*/
    // HAL_SPI_Transmit(&hspi3, (uint8_t *)&drv8301_spi_tx_dat, 1, 100); //dummy transmition to make the clock low

    HAL_GPIO_WritePin(EN_GATE_GPIO_Port, EN_GATE_Pin, GPIO_PIN_SET);

    // HAL_Delay(10);
    // drv8301_spi_tx_dat = DRV8301_GET_SPI_FRAME(DRV8301_MODE_W, DRV8301_ADDRESS_CONTROL2, 0b00000011100);
    // drv8301_spi_tx_dat = DRV8301_GET_SPI_FRAME(DRV8301_MODE_R, DRV8301_ADDRESS_STATUS0, 0b00000000000);
    // drv8301_spi_sent();
    // HAL_Delay(10);

    // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_result_raw, 1);
    // HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&adc_result_raw, 1);
    // HAL_TIM_Base_Start_IT(&htim6);      //for ADC1 and ADC2 sampling triger

    // HAL_Delay(5);
    // current_dc_offset = motor.current - CURRENT_LINEAR_OFFSET;

    // drv8301_spi_tx_dat = DRV8301_GET_SPI_FRAME(DRV8301_MODE_W, DRV8301_ADDRESS_CONTROL2, 0b00000001100);
    // drv8301_spi_sent();
    /*-----------------------------------------------------------------------------------------------------------------*/
    // pid_init(&motor_pid_current, 5, 50, 50, 50.0f, 5.0f, 0.5f);
    pid_init(&motor_pid_speed, -15, 15, 15, 2.8f, 0.0f, 0.0f);
    // pid_init(&motor_pid_position, -50, 50, 50, 3.0f, 0.3f, 0.0f);

    // motor_change_phase();
}

void motor_stop()
{
    phase_select(0);

    HAL_GPIO_WritePin(EN_GATE_GPIO_Port, EN_GATE_Pin, GPIO_PIN_RESET);

    // HAL_ADC_Stop_DMA(&hadc1);
    // HAL_TIM_Base_Stop_IT(&htim3);
}

uint8_t hall;
void motor_change_phase()
{
    hall = read_hall();
    if (motor.pwm_buffer > 0)
        phase_select(hall_state_sequence[7 - read_hall()]);
    else if (motor.pwm_buffer < 0)
        phase_select(hall_state_sequence[read_hall()]);
}

uint32_t last_hall_tick = 0;
void motor_get_speed()
{
    static int32_t last_position = 0;

    motor.rpm = (motor.position - last_position);

    last_position = motor.position;
}

void motor_check_0_speed()
{
    // if (abs(motor.pwm_buffer) < 20 && hall_tick_10us - last_hall_tick > ZERO_SPEED_TIMEOUT)
    //     motor.rpm = 0;

    if (motor.pwm_buffer == 0 && motor.rpm != 0)
        phase_select(0);
    else if (motor.rpm == 0 && motor.pwm_buffer != 0)
        motor_change_phase();
}

void motor_get_position()
{
    static uint8_t last_hall_state;
    uint8_t current_hall_state = read_hall();

    if ((hall_state_sequence[current_hall_state] == 1 && hall_state_sequence[last_hall_state] == 6))
        motor.position++;
    else if (hall_state_sequence[current_hall_state] == 6 && hall_state_sequence[last_hall_state] == 1)
        motor.position--;
    else if (hall_state_sequence[last_hall_state] < hall_state_sequence[current_hall_state])
        motor.position++;
    else if ((hall_state_sequence[last_hall_state] > hall_state_sequence[current_hall_state]))
        motor.position--;

    last_hall_state = current_hall_state;

    motor.degree = HALL2DEGREE(motor.position % POLAR_NUM);
}

void adc_filter()
{
    static uint32_t adc_result_sum = 0;
    static uint16_t adc_filter_cnt = 0;
    adc_result_sum += adc_result_raw;
    adc_filter_cnt++;
    if (adc_filter_cnt > ADC_MEANFILTER_SIZE)
    {
        adc_result = (uint16_t)((float)adc_result_sum / ADC_MEANFILTER_SIZE + 0.5);
        adc_result_sum = 0;
        adc_filter_cnt = 0;
        motor_get_current();
    }
}

void motor_get_current()
{
    motor.current = DRV8301_VOLTAGE2CURRENT(ADC2VOLTAGE(adc_result)) - current_dc_offset;
    // motor.current = ADC2VOLTAGE(adc_result);
}

void motor_current_loop(float set)
{
    motor.pwm = pid_calculate(&motor_pid_current, motor.current, set);
}

void motor_speed_loop(float set)
{
    motor.pwm = pid_calculate(&motor_pid_speed, motor.rpm, set);
    // if (set >= 0)
    //     motor.pwm_buffer = pid_calculate(&motor_pid_speed, motor.rpm, set);
    // else
    //     motor.pwm_buffer = -pid_calculate(&motor_pid_speed, motor.rpm, set);
}

void motor_position_loop(float set)
{
    motor.pwm = pid_calculate(&motor_pid_position, motor.position, set);
}

void motor_pwm_buffer()
{
    // int8_t pwm_diff = motor.pwm - motor.pwm_buffer;
    // if (motor.pwm > 0 && motor.pwm_buffer <= 0)
    // {
    //     if (motor.rpm != 0)
    //         motor.pwm_buffer = 0;
    //     else
    //         motor.pwm_buffer++;
    // }
    // else if (motor.pwm < 0 && motor.pwm_buffer >= 0)
    // {
    //     if (motor.rpm != 0)
    //         motor.pwm_buffer = 0;
    //     else
    //         motor.pwm_buffer--;
    // }

    // int8_t pwm_diff = motor.pwm - motor.pwm_buffer;
    // if (pwm_diff > 0)
    //     motor.pwm_buffer++;
    // else if (pwm_diff < 0)
    //     motor.pwm_buffer--;

    motor.pwm_buffer = motor.pwm;
}