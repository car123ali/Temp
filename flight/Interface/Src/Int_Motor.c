#include "Int_Motor.h"

Motor_TypeDef motor_left_top = {&htim3, TIM_CHANNEL_1, 0};
Motor_TypeDef motor_left_bottom = {&htim4, TIM_CHANNEL_4, 0};
Motor_TypeDef motor_right_top = {&htim2, TIM_CHANNEL_2, 0};
Motor_TypeDef motor_right_bottom = {&htim1, TIM_CHANNEL_3, 0};

void Int_Motor_Start(void){
    HAL_TIM_PWM_Start(motor_left_top.htim, motor_left_top.Channel);
    HAL_TIM_PWM_Start(motor_left_bottom.htim, motor_left_bottom.Channel);
    HAL_TIM_PWM_Start(motor_right_top.htim, motor_right_top.Channel);
    HAL_TIM_PWM_Start(motor_right_bottom.htim, motor_right_bottom.Channel);
}

void Int_Motor_SetSpeed(Motor_TypeDef *motor){
    __HAL_TIM_SET_COMPARE(motor->htim, motor->Channel, motor->Speed);
}