#ifndef __DISPLAY_H
#define __DISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "arm_math.h"

    void Display_Init(void);
    void Display_SendPage(void);
    void Display_WriteBins(float32_t *bins, float32_t max, uint16_t len);

#define BIN_MAX_WIDTH  128
#define BIN_MAX_HEIGHT 64

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H */
