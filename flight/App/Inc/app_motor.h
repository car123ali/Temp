#ifndef __APP_MOTOR_H__
#define __APP_MOTOR_H__

#include "Com_Types.h"
#include "Int_Motor.h"
#include "Int_MPU6050.h"
#include "Com_Filter.h"
#include <stdio.h>
#include "Com_IMU.h"
#include "Com_PID.h"

void App_motor_runtime(PLANE_STATE plane_state, Remote_Struct_t *remote_data);

void App_motor_GetAGFilter(A_G_Struct *ag);

void APP_motor_GetEulerAngle(A_G_Struct *ag, EulerAngle_Struct *eulerAngle, float dt);

void App_motor_pid(Remote_Struct_t *remote_data, A_G_Struct *ag, EulerAngle_Struct *eulerAngle, double dt);

void App_height_pid(uint16_t fix_height, uint16_t current_height, float static_az, float dt);

#endif /* __APP_MOTOR_H__ */
