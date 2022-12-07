#include "main.h"
#include "u8g2.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

extern SPI_HandleTypeDef SPI1_Handle;

uint8_t u8x8_stm32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	/* STM32 supports HW SPI, Remove unused cases like U8X8_MSG_DELAY_XXX & U8X8_MSG_GPIO_XXX */
	switch(msg)
	{
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			break;
		case U8X8_MSG_DELAY_MILLI:
			HAL_Delay(arg_int);
			break;
		case U8X8_MSG_GPIO_RESET:
			HAL_GPIO_WritePin(OLED_RES_Port, OLED_RES_Pin, arg_int);
			break;
		case U8X8_MSG_GPIO_CS:
			HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, arg_int);
			break;
		case U8X8_MSG_GPIO_DC:
			HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, arg_int);
			break;

		default:
			printf("Unknown gpio or delay command - %d: %d\n", msg, arg_int);
			break;
	}
	return 1;
}

uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
			// TODO: Implement IT instead of blocking		
			HAL_SPI_Transmit(&SPI1_Handle, (uint8_t *)arg_ptr, arg_int, 10000);
			break;
		case U8X8_MSG_BYTE_INIT:
			break;
		case U8X8_MSG_BYTE_SET_DC:
			HAL_GPIO_WritePin(OLED_DC_Port, OLED_DC_Pin, arg_int);
			break;
		case U8X8_MSG_BYTE_START_TRANSFER:
			HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, 0);
			break;
		case U8X8_MSG_BYTE_END_TRANSFER:
			HAL_GPIO_WritePin(OLED_CS_Port, OLED_CS_Pin, 1);
			break;
		default:
			printf("Unknown byte command - %d: %d\n", msg, arg_int);
			return 0;
	}
	return 1;
}