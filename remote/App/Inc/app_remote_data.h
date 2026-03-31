#ifndef __APP_REMOTE_DATA_H__
#define __APP_REMOTE_DATA_H__

#include "com_types.h"
#include "Int_Joystick.h"
#include "Int_Button.h"
#include "Int_SI24R1.h"

#define FRAME_HEAD_1 0X43
#define FRAME_HEAD_2 0X33
#define FRAME_HEAD_3 0X12

void App_joystick_data(Remote_Struct_t *remote_data);

void App_key_scan(Remote_Struct_t *remote_data);

COMM_State App_SendData(Remote_Struct_t *remote_data);

#endif /* __APP_REMOTE_DATA_H__ */