#ifndef __STM32F4XX_IOMAPPING_H
#define __STM32F4XX_IOMAPPING_H

#define B1_Pin       GPIO_PIN_0
#define B1_GPIO_Port GPIOA

#define OLED_CS_Pin    GPIO_PIN_0
#define OLED_CS_Port   GPIOB
#define OLED_DC_Pin    GPIO_PIN_1
#define OLED_DC_Port   GPIOB
#define OLED_RES_Pin   GPIO_PIN_2
#define OLED_RES_Port  GPIOB
#define OLED_SCLK_Pin  GPIO_PIN_3
#define OLED_SCLK_Port GPIOB
#define OLED_SDIN_Pin  GPIO_PIN_5
#define OLED_SDIN_Port GPIOB
#define OLED_Port      GPIOB

// #define MEMS_WS_Pin GPIO_PIN_12
// #define MEMS_WS_Port GPIOB
// #define MEMS_CK_Pin GPIO_PIN_13
// #define MEMS_CK_Port GPIOB
// #define MEMS_SD_Pin GPIO_PIN_3
// #define MEMS_SD_Port GPIOC

#define MEMS_WS_Pin  GPIO_PIN_15
#define MEMS_WS_Port GPIOA
#define MEMS_CK_Pin  GPIO_PIN_10
#define MEMS_CK_Port GPIOC
#define MEMS_SD_Pin  GPIO_PIN_12
#define MEMS_SD_Port GPIOC

#define LD4_Pin       GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define LD3_Pin       GPIO_PIN_13
#define LD3_GPIO_Port GPIOD
#define LD5_Pin       GPIO_PIN_14
#define LD5_GPIO_Port GPIOD
#define LD6_Pin       GPIO_PIN_15

#endif /* __STM32F4XX_IOMAPPING_H */