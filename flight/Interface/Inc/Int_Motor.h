#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__

#include "main.h"
#include "tim.h"


// 定义电机控制结构体
typedef struct
{
    TIM_HandleTypeDef *htim;  // 定时器句柄
    uint32_t Channel;        // 通道宏
    uint16_t Speed;          // CCR值，用于设置PWM占空比
} Motor_TypeDef;

extern Motor_TypeDef motor_left_top;
extern Motor_TypeDef motor_left_bottom;
extern Motor_TypeDef motor_right_top;
extern Motor_TypeDef motor_right_bottom;


void Int_Motor_Start(void);

void Int_Motor_SetSpeed(Motor_TypeDef *motor);

#endif /* __INT_MOTOR_H__ */