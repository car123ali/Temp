#ifndef __COM_TYPES_H__
#define __COM_TYPES_H__

#include <stdint.h>

// 枚举类型 遥控状态 未连接 已连接
typedef enum
{
    RC_STATE_DISCONNECTED,
    RC_STATE_CONNECTED
} RC_STATE;

// 枚举类型 飞机状态 空闲 正常 定高 故障
typedef enum
{
    PLANE_STATE_IDLE,
    PLANE_STATE_NORMAL,
    PLANE_STATE_HOLD,
    PLANE_STATE_ERROR
} PLANE_STATE;

typedef enum
{
    THR_FREE,
    THR_MAX,
    THR_LEVEL_MAX,
    THR_MIN,
    THR_UNLOCK
} THR_STATE;

typedef struct
{
    int16_t thr;   // 油门
    int16_t pitch; // 俯仰角
    int16_t roll;  // 横滚角
    int16_t yaw;   // 偏航角
    uint8_t hold;  //  定高
    uint8_t off;   // 关机
} Remote_Struct_t;

typedef enum
{
    RECEIVE_DATA,
    NO_DATA
} RECEIVE_STATE;

typedef enum
{
    LOCK,
    UNLOCK
} THR_LOCK_STATE;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} A_Struct;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} G_Struct;

typedef struct
{
    A_Struct a;
    G_Struct g;
} A_G_Struct;

// 校准数据
typedef struct
{
    int32_t a_x;
    int32_t a_y;
    int32_t a_z;
    int32_t g_x;
    int32_t g_y;
    int32_t g_z;
} A_G_Calibration_Struct;

typedef struct
{
    float yaw;   // 偏航角
    float pitch; // 俯仰角
    float roll;  // 横滚角
} EulerAngle_Struct;

typedef struct
{
    float kp;      // 比例部分系数
    float ki;      // 积分部分系数
    float kd;      // 微分部分系数
    float out;     // PID的输出
    float target;  // 目标值
    float measure; // 测量值 观测值
    float last_error;
    float sum; // 积分值
} PID_Struct;

#endif /* __COM_TYPES_H__ */