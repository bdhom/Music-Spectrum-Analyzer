#include "main.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "display.h"

#include <stdio.h>
#include <stdint.h>

#define SAMPLE_SIZE 256
#define FFT_SIZE    (SAMPLE_SIZE / 2)

extern void initialise_monitor_handles(void);
void SystemClock_Config(void);
static void GPIO_Init(void);
static void SPI_Init(void);
static void copy_array(float32_t *src, float32_t *dest, uint16_t len);

SPI_HandleTypeDef SPI1_Handle = {0};

float32_t TestSineWave[SAMPLE_SIZE] = {
    0.000000,  0.049068,  0.098017,  0.146730,  0.195090,  0.242980,  0.290285,  0.336890,
    0.382683,  0.427555,  0.471397,  0.514103,  0.555570,  0.595699,  0.634393,  0.671559,
    0.707107,  0.740951,  0.773010,  0.803208,  0.831470,  0.857729,  0.881921,  0.903989,
    0.923880,  0.941544,  0.956940,  0.970031,  0.980785,  0.989177,  0.995185,  0.998795,
    1.000000,  0.998795,  0.995185,  0.989177,  0.980785,  0.970031,  0.956940,  0.941544,
    0.923880,  0.903989,  0.881921,  0.857729,  0.831470,  0.803208,  0.773010,  0.740951,
    0.707107,  0.671559,  0.634393,  0.595699,  0.555570,  0.514103,  0.471397,  0.427555,
    0.382683,  0.336890,  0.290285,  0.242980,  0.195090,  0.146730,  0.098017,  0.049068,
    0.000000,  -0.049068, -0.098017, -0.146730, -0.195090, -0.242980, -0.290285, -0.336890,
    -0.382683, -0.427555, -0.471397, -0.514103, -0.555570, -0.595699, -0.634393, -0.671559,
    -0.707107, -0.740951, -0.773010, -0.803208, -0.831470, -0.857729, -0.881921, -0.903989,
    -0.923880, -0.941544, -0.956940, -0.970031, -0.980785, -0.989177, -0.995185, -0.998795,
    -1.000000, -0.998795, -0.995185, -0.989177, -0.980785, -0.970031, -0.956940, -0.941544,
    -0.923880, -0.903989, -0.881921, -0.857729, -0.831470, -0.803208, -0.773010, -0.740951,
    -0.707107, -0.671559, -0.634393, -0.595699, -0.555570, -0.514103, -0.471397, -0.427555,
    -0.382683, -0.336890, -0.290285, -0.242980, -0.195090, -0.146730, -0.098017, -0.049068,
    -0.000000, 0.049068,  0.098017,  0.146730,  0.195090,  0.242980,  0.290285,  0.336890,
    0.382683,  0.427555,  0.471397,  0.514103,  0.555570,  0.595699,  0.634393,  0.671559,
    0.707107,  0.740951,  0.773010,  0.803208,  0.831470,  0.857729,  0.881921,  0.903989,
    0.923880,  0.941544,  0.956940,  0.970031,  0.980785,  0.989177,  0.995185,  0.998795,
    1.000000,  0.998795,  0.995185,  0.989177,  0.980785,  0.970031,  0.956940,  0.941544,
    0.923880,  0.903989,  0.881921,  0.857729,  0.831470,  0.803208,  0.773010,  0.740951,
    0.707107,  0.671559,  0.634393,  0.595699,  0.555570,  0.514103,  0.471397,  0.427555,
    0.382683,  0.336890,  0.290285,  0.242980,  0.195090,  0.146730,  0.098017,  0.049068,
    0.000000,  -0.049068, -0.098017, -0.146730, -0.195090, -0.242980, -0.290285, -0.336890,
    -0.382683, -0.427555, -0.471397, -0.514103, -0.555570, -0.595699, -0.634393, -0.671559,
    -0.707107, -0.740951, -0.773010, -0.803208, -0.831470, -0.857729, -0.881921, -0.903989,
    -0.923880, -0.941544, -0.956940, -0.970031, -0.980785, -0.989177, -0.995185, -0.998795,
    -1.000000, -0.998795, -0.995185, -0.989177, -0.980785, -0.970031, -0.956940, -0.941544,
    -0.923880, -0.903989, -0.881921, -0.857729, -0.831470, -0.803208, -0.773010, -0.740951,
    -0.707107, -0.671559, -0.634393, -0.595699, -0.555570, -0.514103, -0.471397, -0.427555,
    -0.382683, -0.336890, -0.290285, -0.242980, -0.195090, -0.146730, -0.098017, -0.049068};
float32_t Input[SAMPLE_SIZE];
float32_t Output[FFT_SIZE];

int main(void)
{
    float32_t maxValue;
    uint32_t maxIndex;

    initialise_monitor_handles();
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    SPI_Init();
    Display_Init();

    // vTaskStartScheduler();

    while (1)
    {
        copy_array(TestSineWave, Input, SAMPLE_SIZE);

        arm_cfft_f32(&arm_cfft_sR_f32_len128, Input, 0, 1);
        arm_cmplx_mag_f32(Input, Output, FFT_SIZE);
        arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);

        HAL_Delay(33);
        Display_WriteBins(Output, maxValue, FFT_SIZE);
        Display_SendPage();
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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

static void copy_array(float32_t *src, float32_t *dest, uint16_t len)
{
    while (len > 0)
    {
        *dest++ = *src++;
        len--;
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
