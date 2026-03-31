#include "app_motor.h"

PID_Struct pid_pitch = {.kp = -5, .ki = 0, .kd = 0};
PID_Struct pid_pitch_g = {.kp = -2.0, .ki = 0, .kd = -0.3};

PID_Struct pid_roll = {.kp = -5, .ki = 0, .kd = 0};
PID_Struct pid_roll_g = {.kp = -2.0, .ki = 0, .kd = -0.3};

PID_Struct pid_yaw = {.kp = -1, .ki = 0, .kd = 0};
PID_Struct pid_yaw_g = {.kp = 5, .ki = 0, .kd = 0};

PID_Struct pid_h = {.kp = -1.2f, .ki = 0, .kd = -0.085f};
PID_Struct pid_h_v = {.kp = -1.2f, .ki = 0, .kd = -0.085f};

#define LIMIT(a, min, max) ((a) > (max) ? (max) : (a) < (min) ? (min) \
                                                              : (a))

void App_motor_runtime(PLANE_STATE plane_state, Remote_Struct_t *remote_data)
{
    if (plane_state == PLANE_STATE_IDLE)
    {
        motor_left_bottom.Speed = 0;
        motor_left_top.Speed = 0;
        motor_right_bottom.Speed = 0;
        motor_right_top.Speed = 0;
    }
    else if (plane_state == PLANE_STATE_NORMAL)
    {
        // TODO 通过PID进行计算 然后设置马达任务
        motor_left_bottom.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) + pid_pitch_g.out + pid_roll_g.out + pid_yaw_g.out, 0, 1000);
        motor_left_top.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) - pid_pitch_g.out + pid_roll_g.out - pid_yaw_g.out, 0, 1000);
        motor_right_bottom.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) + pid_pitch_g.out - pid_roll_g.out - pid_yaw_g.out, 0, 1000);
        motor_right_top.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) - pid_pitch_g.out - pid_roll_g.out + pid_yaw_g.out, 0, 1000);
    }
    else if (plane_state == PLANE_STATE_HOLD)
    {
        // TODO 根据激光测距设转速q
        motor_left_bottom.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) + pid_pitch_g.out + pid_roll_g.out + pid_yaw_g.out + LIMIT(pid_h_v.out,-150,150), 0, 1000);
        motor_left_top.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) - pid_pitch_g.out + pid_roll_g.out - pid_yaw_g.out + LIMIT(pid_h_v.out,-150,150), 0, 1000);
        motor_right_bottom.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) + pid_pitch_g.out - pid_roll_g.out - pid_yaw_g.out + LIMIT(pid_h_v.out,-150,150), 0, 1000);
        motor_right_top.Speed = LIMIT(LIMIT(remote_data->thr, 0, 800) - pid_pitch_g.out - pid_roll_g.out + pid_yaw_g.out + LIMIT(pid_h_v.out,-150,150), 0, 1000);
    }
    else if (plane_state == PLANE_STATE_ERROR)
    {
        // TODO 着陆算法
        motor_left_bottom.Speed = 0;
        motor_left_top.Speed = 0;
        motor_right_bottom.Speed = 0;
        motor_right_top.Speed = 0;
    }
    if (remote_data->thr < 50)
    {
        motor_left_bottom.Speed = 0;
        motor_left_top.Speed = 0;
        motor_right_bottom.Speed = 0;
        motor_right_top.Speed = 0;
    }

    Int_Motor_SetSpeed(&motor_left_bottom);
    Int_Motor_SetSpeed(&motor_left_top);
    Int_Motor_SetSpeed(&motor_right_bottom);
    Int_Motor_SetSpeed(&motor_right_top);
}

// 在一个全局变量里 保存上一次的低通滤波器的过滤结果
G_Struct last_g = {0};
void App_motor_GetAGFilter(A_G_Struct *ag)
{
    // 1. 读取MPU6050里的加速度数据和角速度数据
    Int_MPU6050_ReadAG_WitchCalbration(ag);

    // printf("%d", ag->g.x);

    // 2. 角速度 -> 因为本身震动不大 使用低通滤波就够了
    ag->g.x = Common_Filter_LowPass(ag->g.x, last_g.x);
    last_g.x = ag->g.x;

    ag->g.y = Common_Filter_LowPass(ag->g.y, last_g.y);
    last_g.y = ag->g.y;

    ag->g.z = Common_Filter_LowPass(ag->g.z, last_g.z);
    last_g.z = ag->g.z;

    // printf(",%d\n", ag->g.x);

    // 3. 加速度计 -> 震动比较大 根据文献 和网上开源资料 我们选择 卡尔曼滤波 (现在不少飞控 用的是陷波滤波器)
    // printf("%d", ag->a.x);
    ag->a.x = Common_Filter_KalmanFilter(&kfs[0], ag->a.x);
    // printf(",%d\n", ag->a.x);
    ag->a.y = Common_Filter_KalmanFilter(&kfs[1], ag->a.y);
    ag->a.z = Common_Filter_KalmanFilter(&kfs[2], ag->a.z);
}

void APP_motor_GetEulerAngle(A_G_Struct *ag, EulerAngle_Struct *eulerAngle, float dt)
{
    Common_IMU_GetEulerAngle(ag, eulerAngle, dt);
}

void App_motor_pid(Remote_Struct_t *remote_data, A_G_Struct *ag, EulerAngle_Struct *eulerAngle, double dt)
{
    pid_pitch.target = (remote_data->pitch - 500) * 0.02;
    pid_pitch.measure = eulerAngle->pitch;
    pid_pitch_g.measure = ag->g.y * 4000.0 / 65535;
    Com_PID_Serial(&pid_pitch, &pid_pitch_g, dt);

    pid_roll.target = (remote_data->roll - 500) * 0.02;
    pid_roll.measure = eulerAngle->roll;
    pid_roll_g.measure = ag->g.x * 4000.0 / 65535;
    Com_PID_Serial(&pid_roll, &pid_roll_g, dt);

    pid_yaw.target = (remote_data->yaw - 500) * 0.02;
    pid_yaw.measure = eulerAngle->yaw;
    pid_yaw_g.measure = ag->g.z * 4000.0 / 65535;
    Com_PID_Serial(&pid_yaw, &pid_yaw_g, dt);
}

uint16_t last_height = 0;
float speed2 = 0;

void App_height_pid(uint16_t fix_height, uint16_t current_height, float static_az, float dt)
{
    // 外环的目标值和当前值
    pid_h.target = fix_height;
    pid_h.measure = current_height;

    // // 算内环的垂直方向速度
    float speed1 = (current_height - last_height) / dt;
    last_height = current_height;

    // 加速度求积分
    speed2 += (Common_IMU_GetNormAccZ() - static_az) * dt;

    pid_h_v.measure = (0.98 * speed2 + 0.02 * speed1);
    speed2 = (0.98 * speed2 + 0.02 * speed1);

    Com_PID_Serial(&pid_h, &pid_h_v, dt);
}