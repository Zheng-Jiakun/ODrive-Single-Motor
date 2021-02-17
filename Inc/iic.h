#ifndef _IIC_H_
#define _IIC_H_

#include "main.h"
#include "user_main.h"

typedef struct {
	GPIO_TypeDef *SDA_GPIO_Port;
	uint16_t SDA_GPIO_Pin;

	GPIO_TypeDef *SCL_GPIO_Port;
	uint16_t SCL_GPIO_Pin;
} IIC_GPIO_t;

void iic_init (void);
void iic_send_start (void);
void iic_send_stop (void);
void iic_send_ack (void);
void iic_send_nack (void);
void iic_send_byte (uint8_t dat);
void iic_read_byte (uint8_t *dat);
uint8_t iic_wait_ack (void);

#endif
