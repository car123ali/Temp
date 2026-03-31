#ifndef __INT_LED_H__
#define __INT_LED_H__

#include "main.h"

// 定义一个结构体类型 里面包括LED的GPIO端口和引脚号
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint32_t last_toggle_tick;
} LED_TypeDef;

extern LED_TypeDef led_left_bottom;
extern LED_TypeDef led_left_top;
extern LED_TypeDef led_right_bottom;
extern LED_TypeDef led_right_top;


void LED_On(LED_TypeDef *led);

void LED_Off(LED_TypeDef *led);

void LED_Blink(LED_TypeDef *led,uint32_t tick);

#endif /* __INT_LED_H__ */