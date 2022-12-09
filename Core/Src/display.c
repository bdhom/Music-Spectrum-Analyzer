#include "display.h"

#include "u8g2.h"

extern uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
extern uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

static u8g2_t u8g2 = {0};

void Display_Init(void)
{
  	u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
  	u8g2_InitDisplay(&u8g2);
  	u8g2_SetPowerSave(&u8g2, 0);

  	u8g2_FirstPage(&u8g2);
}

void Display_SendPage(void)
{
	u8g2_NextPage(&u8g2);
	u8g2_ClearBuffer(&u8g2);
}

void Display_WriteBins(uint8_t* bins, uint8_t length)
{
	uint8_t position, height;
	uint8_t width = BIN_MAX_WIDTH / length;

	if (width == 0) return;

	for (int i = 0; i < length; i++)
	{
		position = i * width;
		height = (bins[i] > BIN_MAX_HEIGHT) ? BIN_MAX_HEIGHT : bins[i];
		u8g2_DrawBox(&u8g2, position, BIN_MAX_HEIGHT - height, width, height);
	}
}
