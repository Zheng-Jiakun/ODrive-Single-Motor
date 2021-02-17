#include <WS2812B.h>

#define PREFIX_NUM      10
#define CODE_0          0xc0
#define CODE_1          0xf0

#define FRAME_SIZE (PREFIX_NUM + 24 * LED_NUM + 200)

color_t led_string[LED_NUM] = {0};

uint8_t DMADataBuf[FRAME_SIZE];

void WS2812_init()
{
    HAL_SPI_Transmit_DMA(&hspi3, DMADataBuf, FRAME_SIZE);
}

void WS2812_update()
{
    // memset(DMADataBuf, 0, sizeof(DMADataBuf));

    for (uint16_t j = 0; j < LED_NUM; j++)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            if (led_string[j].g & (0x80 >> i))
                DMADataBuf[PREFIX_NUM + j * 24 + i] = CODE_1;
            else
                DMADataBuf[PREFIX_NUM + j * 24 + i] = CODE_0;
        }
        for (uint8_t i = 0; i < 8; i++)
        {
            if (led_string[j].r & (0x80 >> i))
                DMADataBuf[PREFIX_NUM + j * 24 + i + 8] = CODE_1;
            else
                DMADataBuf[PREFIX_NUM + j * 24 + i + 8] = CODE_0;
        }
        for (uint8_t i = 0; i < 8; i++)
        {
            if (led_string[j].b & (0x80 >> i))
                DMADataBuf[PREFIX_NUM + j * 24 + i + 16] = CODE_1;
            else
                DMADataBuf[PREFIX_NUM + j * 24 + i + 16] = CODE_0;
        }
    }
    // HAL_SPI_Transmit(&hspi3, DMADataBuf, FRAME_SIZE, 100);
}