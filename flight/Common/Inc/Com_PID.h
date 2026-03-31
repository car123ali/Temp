#ifndef __COM_PID_H__
#define __COM_PID_H__
#include "Com_Types.h"

// delta time
void Com_SinglePID(PID_Struct *PID_Struct, float dt);

void Com_PID_Serial(PID_Struct *outter_pid, PID_Struct *inner_pid, float dt);
#endif /* __COM_PID_H__ */