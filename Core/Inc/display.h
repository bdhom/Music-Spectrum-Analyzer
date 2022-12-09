#ifndef __DISPLAY_H
#define __DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Display_Init(void);
void Display_SendPage(void);
void Display_WriteBins(uint8_t* bins, uint8_t length);

#define BIN_MAX_WIDTH  128
#define BIN_MAX_HEIGHT 64

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H */
