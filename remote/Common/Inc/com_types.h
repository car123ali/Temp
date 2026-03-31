#ifndef __COM_TYPES_H__
#define __COM_TYPES_H__

#include <stdint.h>

typedef enum
{
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_LEFT_X,
    KEY_RIGHT_X,
    KEY_LEFT_X_LONG,
    KEY_RIGHT_X_LONG,
    KEY_NONE
} KEY_Type;

// 遥控数据
// 油门
// 定高(0,1)
// 关机(0,1)
// 俯仰角
// 横滚角
// 偏航角
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
    COMM_OK,
    COMM_ERROR
} COMM_State;

#endif /* __COM_TYPES_H__ */