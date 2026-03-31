#ifndef __APP_REMOTE_DATA_H__
#define __APP_REMOTE_DATA_H__

#include "Int_SI24R1.h"
#include "Com_Types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Com_IMU.h"

#define FRAME_HEAD_1 0X43
#define FRAME_HEAD_2 0X33
#define FRAME_HEAD_3 0X12

RECEIVE_STATE App_remote_receive_data(Remote_Struct_t *remote_data);

void App_remote_handleRCConnectState(RECEIVE_STATE receive_state, RC_STATE *rc_state);

THR_LOCK_STATE App_remote_handleThrUnlock(Remote_Struct_t *remote_data);

void App_remote_handleFlightState(Remote_Struct_t *remote_data, RC_STATE rcState, PLANE_STATE *planeState);

void App_handlePowerOff(Remote_Struct_t *remote_data, TaskHandle_t *task_Handle);

#endif /* __APP_REMOTE_DATA_H__ */