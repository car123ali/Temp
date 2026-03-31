#ifndef __FLIGH_TASK_H__
#define __FLIGH_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "Int_IP5305T.h"
#include "Int_SI24R1.h"
#include "Int_Button.h"
#include "com_types.h"
#include "app_remote_data.h"
#include "app_display.h"

// 开启flight所有任务
void Remote_TaskStart(void);

#endif /* __FLIGH_TASK_H__ */