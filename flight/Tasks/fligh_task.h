#ifndef __FLIGH_TASK_H__
#define __FLIGH_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "Int_IP5305T.h"
#include "Com_types.h"
#include "Int_LED.h"
#include "Int_Motor.h"
#include "Int_SI24R1.h"
#include "string.h"
#include "app_remote_data.h"
#include "app_motor.h"
#include "Int_MPU6050.h"
#include "Int_VL53L1X.h"

// 开启flight所有任务
void Flight_TaskStart(void);

#endif /* __FLIGH_TASK_H__ */