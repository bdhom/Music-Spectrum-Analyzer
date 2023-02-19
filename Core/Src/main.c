#include "main.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "display.h"

#include <stdio.h>
#include <stdint.h>

#define MAX(A, B)   (((A) > (B)) ? (A) : (B))
#define ZERO_MIN(A) MAX(A, 0)

#define MEMS_SAMPLE_FREQ 48387.0f
#define BIN_TOTAL        10
#define FFT_SIZE         2048
#define FREQ_COUNT       (FFT_SIZE / 2)
#define FREQ_PER_COUNT   (MEMS_SAMPLE_FREQ / FFT_SIZE) // ~23.63 Hz / count

#define BIN_CEILING 300.0f
#define BIN_FLOOR   300.0f

extern void initialise_monitor_handles(void);
static void SystemClock_Config(void);
static void GPIO_Init(void);
static void DMA_Init(void);
static void SPI_Init(void);
static void I2S_Init(void);

static void process_samples(uint16_t *rx);
static void fill_bins(void);

arm_rfft_fast_instance_f32 FFT_Handle;
SPI_HandleTypeDef SPI1_Handle = {0};
I2S_HandleTypeDef I2S3_Handle = {0};
DMA_HandleTypeDef DMA_I2S3_Rx_Handle = {0};

uint8_t rxBufState = 0;

uint16_t rxBuf[8 * FFT_SIZE];
float32_t fftInBuf[FFT_SIZE];
float32_t fftOutBuf[FFT_SIZE];
float32_t freqs[FREQ_COUNT];
float32_t bins[BIN_TOTAL];
uint16_t freqIndices[BIN_TOTAL] = {1, 2, 5, 10, 25, 50, 100, 200, 500, 1000};

int main(void)
{
    uint16_t *rx;

    initialise_monitor_handles();
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    DMA_Init();
    SPI_Init();
    I2S_Init();
    Display_Init();

    HAL_I2S_Receive_DMA(&I2S3_Handle, rxBuf, 4 * FFT_SIZE);

    arm_rfft_fast_init_f32(&FFT_Handle, FFT_SIZE);

    // vTaskStartScheduler();

    while (1)
    {
        if (rxBufState != 0)
        {
            rx = (rxBufState == 1) ? &rxBuf[0] : &rxBuf[4 * FFT_SIZE];

            process_samples(rx);
            fill_bins();
            Display_WriteBins(bins, BIN_CEILING, BIN_TOTAL);
            Display_SendPage();

            rxBufState = 0;
        }
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the PLL clock source for I2S
     */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
    PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, LD4_Pin | LD3_Pin | LD5_Pin | LD6_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin */
    GPIO_InitStruct.Pin = LD4_Pin | LD3_Pin | LD5_Pin | LD6_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
 * @brief DMA Initialization Function
 * @param None
 * @retval None
 */
static void DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief SPI Initialization Function
 * @param None
 * @retval None
 */
static void SPI_Init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();

    SPI_InitTypeDef SPI_Init = {0};

    SPI_Init.Mode = SPI_MODE_MASTER;
    SPI_Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Init.CLKPolarity = SPI_POLARITY_HIGH;
    SPI_Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Init.NSS = SPI_NSS_SOFT;
    SPI_Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;

    SPI1_Handle.Init = SPI_Init;
    SPI1_Handle.Instance = SPI1;

    if (HAL_SPI_Init(&SPI1_Handle) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief I2S Initialization Function
 * @param None
 * @retval None
 */
static void I2S_Init()
{
    __HAL_RCC_SPI3_CLK_ENABLE();

    I2S_InitTypeDef I2S_InitStruct = {0};

    I2S_InitStruct.Mode = I2S_MODE_MASTER_RX;
    I2S_InitStruct.AudioFreq = I2S_AUDIOFREQ_48K;
    I2S_InitStruct.ClockSource = I2S_CLOCK_PLL;
    I2S_InitStruct.CPOL = I2S_CPOL_LOW;
    I2S_InitStruct.DataFormat = I2S_DATAFORMAT_24B;
    I2S_InitStruct.Standard = I2S_STANDARD_MSB;
    I2S_InitStruct.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    I2S_InitStruct.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;

    I2S3_Handle.Init = I2S_InitStruct;
    I2S3_Handle.Instance = SPI3;

    if (HAL_I2S_Init(&I2S3_Handle) != HAL_OK)
    {
        Error_Handler();
    }
}

static void process_samples(uint16_t *rx)
{
    // Mono-sum to input
    for (int i = 0; i < FFT_SIZE; i++)
    {
        uint32_t temp = (uint32_t)(*rx++ << 16);
        temp |= *rx++;
        uint32_t temp2 = (uint32_t)(*rx++ << 16);
        temp2 |= *rx++;

        fftInBuf[i] = (float32_t)temp + (float32_t)temp2;
    }

    arm_rfft_fast_f32(&FFT_Handle, fftInBuf, fftOutBuf, 0);
    arm_cmplx_mag_f32(fftOutBuf, freqs, FREQ_COUNT);
}

static void fill_bins(void)
{
    for (int i = 0; i < BIN_TOTAL; i++)
    {
        bins[i] = 20.0f * logf(freqs[freqIndices[i]]) - BIN_FLOOR;
        bins[i] = ZERO_MIN(bins[i]);
    }
}

// TODO: Implement callback
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    while (1)
    {
    }
}

// TODO: Implement callback
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    rxBufState = 1;
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    rxBufState = 2;
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();

    printf("Unhandled error trap\n");
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */
