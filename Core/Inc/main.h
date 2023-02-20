#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ioMapping.h"

#define MAX(A, B)   (((A) > (B)) ? (A) : (B))
#define MIN(A, B)   (((A) < (B)) ? (A) : (B))
#define ZERO_MIN(A) MAX(A, 0)

#define MEMS_SAMPLE_FREQ 48387.0f
#define BIN_TOTAL        10
#define FFT_SIZE         2048
#define FREQ_COUNT       (FFT_SIZE / 2)
#define FREQ_PER_COUNT   (MEMS_SAMPLE_FREQ / FFT_SIZE) // ~23.63 Hz / count w/ 48387 Hz @ 2048

#define BIN_CEILING 300.0f
#define BIN_FLOOR   300.0f

    void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
