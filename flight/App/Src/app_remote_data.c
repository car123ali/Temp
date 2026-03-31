#include "app_remote_data.h"

uint8_t rx_buffer[17];
RECEIVE_STATE App_remote_receive_data(Remote_Struct_t *remote_data)
{
    // 1. 先收到数据放到buffer
    uint8_t result = Int_SI24R1_RxPacket(rx_buffer);
    if (result == 0)
    {
        // 2. 先判断地址是不是自己的
        if ((rx_buffer[0] == FRAME_HEAD_1) && (rx_buffer[1] == FRAME_HEAD_2) && (rx_buffer[2] == FRAME_HEAD_3))
        {
            printf("is mine\n");
            // 3. 再判断校验和是否完整
            uint32_t x = 0;
            for (uint8_t i = 0; i < 13; i++)
            {
                x += rx_buffer[i];
            }
            uint32_t rx_x = 0;
            rx_x = rx_buffer[13] << 24;
            rx_x |= rx_buffer[14] << 16;
            rx_x |= rx_buffer[15] << 8;
            rx_x |= rx_buffer[16];
            if (x == rx_x)
            {
                remote_data->thr = rx_buffer[3] << 8;
                remote_data->thr += rx_buffer[4];
                remote_data->pitch = rx_buffer[5] << 8;
                remote_data->pitch += rx_buffer[6];
                remote_data->roll = rx_buffer[7] << 8;
                remote_data->roll += rx_buffer[8];
                remote_data->yaw = rx_buffer[9] << 8;
                remote_data->yaw += rx_buffer[10];

                remote_data->hold = rx_buffer[11];
                remote_data->off = rx_buffer[12];
                return RECEIVE_DATA;
            }
        }
    }
    return NO_DATA;

    // 4. 如果上面的内容都正确 则将数据赋值给remote_data1
    // 5. 验证：遥控端发送数据 飞控段打印remote_data里的数据 要跟着变。
}

// 上一次收到数据的时间
uint32_t last_receive_tick = 0;
void App_remote_handleRCConnectState(RECEIVE_STATE receive_state, RC_STATE *rc_state)
{
    // 如果收到数据 就更新最近一次收到数据的时间 并将状态设为连接
    if (receive_state == RECEIVE_DATA)
    {
        last_receive_tick = xTaskGetTickCount();
        *rc_state = RC_STATE_CONNECTED;
    }
    // 如果没有收到数据 并且超过1s 则将状态设为断开
    else if (receive_state == NO_DATA)
    {
        if ((xTaskGetTickCount() - last_receive_tick) > 1000)
        {
            *rc_state = RC_STATE_DISCONNECTED;
        }
    }
}

THR_STATE thr_state = THR_FREE;
uint32_t max_state_start_tick = 0;
uint32_t min_state_start_tick = 0;
THR_LOCK_STATE App_remote_handleThrUnlock(Remote_Struct_t *remote_data)
{
    switch (thr_state)
    {
    case THR_FREE:
        if (remote_data->thr > 900)
        {
            thr_state = THR_MAX;
            max_state_start_tick = xTaskGetTickCount();
        }
        break;
    case THR_MAX:
        if (remote_data->thr < 900)
        {
            if (xTaskGetTickCount() - max_state_start_tick >= 1000)
            {
                thr_state = THR_LEVEL_MAX;
            }
            else
            {
                thr_state = THR_FREE;
            }
        }
        break;
    case THR_LEVEL_MAX:
        if (remote_data->thr < 100)
        {
            thr_state = THR_MIN;
            min_state_start_tick = xTaskGetTickCount();
        }
        break;
    case THR_MIN:
        if (xTaskGetTickCount() - min_state_start_tick > 1000)
        {
            thr_state = THR_UNLOCK;
        }
        else
        {
            if (remote_data->thr > 100)
            {
                thr_state = THR_FREE;
            }
        }
        break;
    case THR_UNLOCK:
        thr_state = THR_FREE;
        return UNLOCK;
    }
    return LOCK;
}

extern uint16_t height;

// 定高目标值
extern uint16_t fix_height;

extern float static_az;

void App_remote_handleFlightState(Remote_Struct_t *remote_data, RC_STATE rcState, PLANE_STATE *planeState)
{
    switch (*planeState)
    {
    case PLANE_STATE_IDLE:
        if (rcState == RC_STATE_CONNECTED)
        {
            if (App_remote_handleThrUnlock(remote_data) == UNLOCK)
            {
                *planeState = PLANE_STATE_NORMAL;
                static_az = Common_IMU_GetNormAccZ();
            }
        }
        printf("idle\n");
        break;
    case PLANE_STATE_NORMAL:
        if (rcState == RC_STATE_DISCONNECTED)
        {
            *planeState = PLANE_STATE_ERROR;
        }
        else if (rcState == RC_STATE_CONNECTED)
        {
            if (remote_data->hold == 1)
            {
                *planeState = PLANE_STATE_HOLD;
                // 让fix_hight = 当前的高度
                fix_height = height;
            }
        }
        printf("normal\n");
        break;
    case PLANE_STATE_HOLD:
        if (rcState == RC_STATE_DISCONNECTED)
        {
            *planeState = PLANE_STATE_ERROR;
        }
        else if (rcState == RC_STATE_CONNECTED)
        {
            if (remote_data->hold == 1)
            {
                *planeState = PLANE_STATE_NORMAL;
            }
        }
        printf("hold\n");
        break;
    case PLANE_STATE_ERROR:
        // 等待重新连接
        if (rcState == RC_STATE_CONNECTED)
        {
            *planeState = PLANE_STATE_IDLE;
        }
        printf("error\n");
        break;
    }
}

void App_handlePowerOff(Remote_Struct_t *remote_data, TaskHandle_t *task_Handle)
{
    if (remote_data->off == 1)
    {
        xTaskNotifyGive(*task_Handle);
    }
}