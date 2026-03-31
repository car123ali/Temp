#include "Com_PID.h"

// 单PID
void Com_SinglePID(PID_Struct *PID_Struct, float dt)
{
    // 先计算当前观测值和实际值之间的误差 ek
    float current_error = PID_Struct->measure - PID_Struct->target;
    // 计算微分部分
    float d_error = (current_error - PID_Struct->last_error) / dt;
    // 计算积分部分
    PID_Struct->sum += current_error * dt;
    // 将本次的error 当作下一次计算的last_error
    PID_Struct->last_error = current_error;
    // 将计算结果保存在结构体的out之中 当作本次计算的输出
    PID_Struct->out = PID_Struct->kp * current_error + PID_Struct->ki * PID_Struct->sum + PID_Struct->kd * d_error;
}

// 串级PID
void Com_PID_Serial(PID_Struct *outter_pid, PID_Struct *inner_pid, float dt)
{
    Com_SinglePID(outter_pid, dt);
    inner_pid->target = outter_pid->out;
    Com_SinglePID(inner_pid, dt);
}