#ifndef _WS2812B_H_
#define _WS2812B_H_

#include "main.h"
#include "dma.h"
#include "tim.h"
#include "spi.h"
#include "string.h"

#define LED_NUM     8

typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

extern color_t led_string[LED_NUM];
void WS2812_init ();
void WS2812_update ();

#endif