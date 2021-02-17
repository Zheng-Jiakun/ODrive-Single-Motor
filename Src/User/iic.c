#include "iic.h"
#include "gpio.h"

IIC_GPIO_t IIC_GPIO;

#define IIC_SET_SCL() HAL_GPIO_WritePin(IIC_GPIO.SCL_GPIO_Port, IIC_GPIO.SCL_GPIO_Pin, GPIO_PIN_SET)
#define IIC_RESET_SCL() HAL_GPIO_WritePin(IIC_GPIO.SCL_GPIO_Port, IIC_GPIO.SCL_GPIO_Pin, GPIO_PIN_RESET)
#define IIC_SET_SDA() HAL_GPIO_WritePin(IIC_GPIO.SDA_GPIO_Port, IIC_GPIO.SDA_GPIO_Pin, GPIO_PIN_SET)
#define IIC_RESET_SDA() HAL_GPIO_WritePin(IIC_GPIO.SDA_GPIO_Port, IIC_GPIO.SDA_GPIO_Pin, GPIO_PIN_RESET)
#define IIC_READ_SDA() HAL_GPIO_ReadPin(IIC_GPIO.SDA_GPIO_Port, IIC_GPIO.SDA_GPIO_Pin)

void delay_us(uint16_t us)
{
	volatile uint16_t tus = us * 10;
	while (tus--)
		;
}

void iic_sda_out()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = IIC_GPIO.SDA_GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(IIC_GPIO.SDA_GPIO_Port, &GPIO_InitStruct);
}

void iic_sda_in()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = IIC_GPIO.SDA_GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(IIC_GPIO.SDA_GPIO_Port, &GPIO_InitStruct);
}

void iic_config_sda(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	IIC_GPIO.SDA_GPIO_Port = GPIOx;
	IIC_GPIO.SDA_GPIO_Pin = GPIO_Pin;
}

void iic_config_scl(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	IIC_GPIO.SCL_GPIO_Port = GPIOx;
	IIC_GPIO.SCL_GPIO_Pin = GPIO_Pin;
}

void iic_init(void)
{
	iic_config_scl(IMU_SCL_GPIO_Port, IMU_SCL_Pin);
	iic_config_sda(IMU_SDA_GPIO_Port, IMU_SDA_Pin);
}

void iic_send_start(void)
{
	iic_sda_out();
	IIC_SET_SDA();
	IIC_SET_SCL();
	delay_us(4);
	IIC_RESET_SDA();
	delay_us(4);
	IIC_RESET_SCL();
	delay_us(2);
}

void iic_send_stop(void)
{
	iic_sda_out();
	IIC_RESET_SCL();
	IIC_RESET_SDA();
	delay_us(4);
	IIC_SET_SCL();
	delay_us(4);
	IIC_SET_SDA();
	delay_us(4);
}

void iic_send_byte(uint8_t dat)
{
	iic_sda_out();
	IIC_RESET_SCL();
	for (uint8_t temp = 0x80; temp != 0; temp >>= 1)
	{
		if ((temp & dat) == 0)
			IIC_RESET_SDA();
		else
			IIC_SET_SDA();
		delay_us(2);
		IIC_SET_SCL();
		delay_us(2);
		IIC_RESET_SCL();
		delay_us(2);
	}
}

void iic_read_byte(uint8_t *dat)
{
	iic_sda_in();
	for (uint8_t temp = 0x80; temp != 0; temp >>= 1)
	{
		IIC_RESET_SCL();
		delay_us(2);
		IIC_SET_SCL();
		if (IIC_READ_SDA() == GPIO_PIN_SET)
			*dat |= temp;
		else
			*dat &= ~temp;
		delay_us(1);
	}
}

uint8_t iic_wait_ack(void)
{
	uint16_t cnt = 0;
	IIC_SET_SDA();
	delay_us(1);
	IIC_SET_SCL();
	delay_us(1);
	iic_sda_in();
	while (IIC_READ_SDA() == GPIO_PIN_SET)
	{
		cnt++;
		// delay_us(1);
		if (cnt >= 250) //200us
		{
			iic_send_stop();
			return 1;
		}
	}
	IIC_RESET_SCL();
	return 0;
}

void iic_send_ack(void)
{
	iic_sda_out();
	IIC_RESET_SCL();
	IIC_RESET_SDA();
	delay_us(2);
	IIC_SET_SCL();
	delay_us(2);
	IIC_RESET_SCL();
}

void iic_send_nack(void)
{
	iic_sda_out();
	IIC_RESET_SCL();
	IIC_SET_SDA();
	delay_us(2);
	IIC_SET_SCL();
	delay_us(2);
	IIC_RESET_SCL();
}

// void eeprom_write_byte(uint16_t address, uint8_t val)
// {
// 	iic_send_start();

// 	iic_send_byte(0xA0 + ((address / 256) << 1));
// 	iic_wait_ack();

// 	iic_send_byte(address % 256);
// 	iic_wait_ack();

// 	iic_send_byte(val);
// 	iic_wait_ack();

// 	iic_send_stop();

// 	HAL_Delay(20);
// }

// void eeprom_read_byte(uint16_t address, uint8_t *val)
// {
// 	iic_send_start();

// 	iic_send_byte(0xA0 + ((address / 256) << 1));
// 	iic_wait_ack();

// 	iic_send_byte(address % 256);
// 	iic_wait_ack();

// 	iic_send_start();

// 	iic_send_byte(0xA1);
// 	iic_wait_ack();

// 	iic_read_byte(val);
// 	iic_send_nack();

// 	iic_send_stop();
// }
