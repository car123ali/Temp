#include "Int_LED.h"

LED_TypeDef led_left_bottom = {LED_LEFT_BOTTOM_GPIO_Port, LED_LEFT_BOTTOM_Pin, 0};
LED_TypeDef led_left_top = {LED_LEFT_TOP_GPIO_Port, LED_LEFT_TOP_Pin, 0};
LED_TypeDef led_right_bottom = {LED_RIGHT_BOTTOM_GPIO_Port, LED_RIGHT_BOTTOM_Pin, 0};
LED_TypeDef led_right_top = {LED_RIGHT_TOP_GPIO_Port, LED_RIGHT_TOP_Pin, 0};

void LED_On(LED_TypeDef *led){
    HAL_GPIO_WritePin(led->GPIOx,led->GPIO_Pin,GPIO_PIN_RESET);
}

void LED_Off(LED_TypeDef *led){
    HAL_GPIO_WritePin(led->GPIOx,led->GPIO_Pin,GPIO_PIN_SET);
}

void LED_Blink(LED_TypeDef *led,uint32_t tick){
    HAL_GPIO_TogglePin(led->GPIOx,led->GPIO_Pin);
    led->last_toggle_tick = tick;
}