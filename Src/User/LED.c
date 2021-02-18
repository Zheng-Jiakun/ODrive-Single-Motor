#include "LED.h"

void LED_init()
{
    WS2812_init();
    for (uint8_t i = 0; i < LED_NUM / 2; i++)
    {
        led_string[i + 4].r = 50;
        led_string[i + 4].g = 50;
        led_string[i + 4].b = 50;
        led_string[LED_NUM / 2 - 1 - i].r = 50;
        led_string[LED_NUM / 2 - 1 - i].g = 50;
        led_string[LED_NUM / 2 - 1 - i].b = 50;
        WS2812_update();
        HAL_Delay(200);
    }
}

void LED_stop()
{
    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        led_string[i].r = 255;
        led_string[i].g = 0;
        led_string[i].b = 0;
    }
    WS2812_update();
}

void LED_go()
{
    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        led_string[i].r = 0;
        led_string[i].g = 255;
        led_string[i].b = 255;
    }
    WS2812_update();
}

void LED_left()
{
    for (uint8_t i = 0; i < LED_NUM / 2; i++)
    {
        led_string[i].r = 255;
        led_string[i].g = 255;
        led_string[i].b = 0;
    }
    WS2812_update();
}

void LED_right()
{
    for (uint8_t i = 4; i < LED_NUM; i++)
    {
        led_string[i].r = 255;
        led_string[i].g = 255;
        led_string[i].b = 0;
    }
    WS2812_update();
}

void LED_off()
{
    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        led_string[i].r = 0;
        led_string[i].g = 0;
        led_string[i].b = 0;
    }
    WS2812_update();
}

void LED_system_start(float a)
{
    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        led_string[i].r = 0;
        led_string[i].g = 0;
        led_string[i].b = 0;
    }
    led_string[(uint8_t)(a / 90.0f * 7 + 7)].r = 255;
    led_string[(uint8_t)(a / 90.0f * 7 + 7)].g = 255;
    led_string[(uint8_t)(a / 90.0f * 7 + 7)].b = 255;
    WS2812_update();
}