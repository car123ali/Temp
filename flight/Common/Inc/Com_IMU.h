#ifndef __COMMON_IMU_H
#define __COMMON_IMU_H

#include "math.h"
#include "Com_Types.h"

/* 表示四元数的结构体 */
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_Struct;

extern float RtA;
extern float Gyro_G;
extern float Gyro_Gr;

void Common_IMU_GetEulerAngle(A_G_Struct *gyroAccel,
                              EulerAngle_Struct *eulerAngle,
                              float dt);
float Common_IMU_GetNormAccZ(void);

#endif