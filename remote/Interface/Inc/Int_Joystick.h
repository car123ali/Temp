#ifndef __INT_JOYSTICK_H__
#define __INT_JOYSTICK_H__

#include "main.h"
#include "adc.h"
#include "com_types.h"

void Int_Joystick_Init(void);

void Int_Joystick_Scan(Remote_Struct_t *remote_data);

#endif /* __INT_JOYSTICK_H__ */