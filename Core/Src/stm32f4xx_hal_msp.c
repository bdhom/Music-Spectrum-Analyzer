#include "main.h"

extern DMA_HandleTypeDef DMA_I2S3_Rx_Handle;

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin : OLED_CS_Pin OLED_DC_Pin OLED_RES_Pin */
    GPIO_InitStruct.Pin = OLED_CS_Pin | OLED_DC_Pin | OLED_RES_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : OLED_SCLK_Pin OLED_SDIN_Pin */
    GPIO_InitStruct.Pin = OLED_SCLK_Pin | OLED_SDIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin : MEMS_WS_Pin */
    GPIO_InitStruct.Pin = MEMS_WS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3; // I2S3
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : MEMS_CK_Pin MEMS_SD_Pin */
    GPIO_InitStruct.Pin = MEMS_CK_Pin | MEMS_SD_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3; // I2S3
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* I2S3 DMA Init */
    DMA_I2S3_Rx_Handle.Instance = DMA1_Stream0;
    DMA_I2S3_Rx_Handle.Init.Channel = DMA_CHANNEL_0;
    DMA_I2S3_Rx_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    DMA_I2S3_Rx_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    DMA_I2S3_Rx_Handle.Init.MemInc = DMA_MINC_ENABLE;
    DMA_I2S3_Rx_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DMA_I2S3_Rx_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    DMA_I2S3_Rx_Handle.Init.Mode = DMA_CIRCULAR;
    DMA_I2S3_Rx_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    DMA_I2S3_Rx_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&DMA_I2S3_Rx_Handle) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(hi2s, hdmarx, DMA_I2S3_Rx_Handle);
}
