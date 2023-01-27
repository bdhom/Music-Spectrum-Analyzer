#include "display.h"

#include "u8g2.h"

#define min(A, B) (((A) < (B)) ? (A) : (B))

extern uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

static u8g2_t u8g2 = {0};

void Display_Init(void)
{
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi,
                                       u8x8_stm32_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    u8g2_FirstPage(&u8g2);
}

void Display_SendPage(void)
{
    u8g2_NextPage(&u8g2);
    u8g2_ClearBuffer(&u8g2);
}

inline void Display_WriteBin(uint8_t height, uint8_t width, uint8_t offset)
{
    u8g2_DrawBox(&u8g2, offset, BIN_MAX_HEIGHT - height, width, height);
}

void Display_WriteBins(float32_t *bins, float32_t max, uint16_t len)
{
    uint8_t height, width, offset;
    uint16_t rows = min(len, BIN_MAX_WIDTH);

    width = BIN_MAX_WIDTH / rows;

    for (uint8_t i = 0; i < rows; i++)
    {
        offset = i * width;
        height = ((float32_t)BIN_MAX_HEIGHT * bins[i]) / max;
        Display_WriteBin(height, width, offset);
    }
}
