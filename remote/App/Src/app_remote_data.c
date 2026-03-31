#include "app_remote_data.h"

int16_t roll_offset = 0;
int16_t pitch_offset = 0;

int16_t thr_offset = 0;
int16_t yaw_offset = 0;

uint8_t tx_buffer[17];

void App_joystick_data(Remote_Struct_t *remote_data)
{
    taskENTER_CRITICAL();
    // 1. 获取摇杆的数据
    Int_Joystick_Scan(remote_data);
    // 2. 处理方向和映射关系 0-1000范围内 而且方向还得修改
    remote_data->yaw = (4095 - remote_data->yaw) * 1000 / 4095;
    remote_data->pitch = (4095 - remote_data->pitch) * 1000 / 4095;
    remote_data->roll = (4095 - remote_data->roll) * 1000 / 4095;
    remote_data->thr = (4095 - remote_data->thr) * 1000 / 4095;

    // 3. 处理中点的微调
    remote_data->roll += roll_offset;
    remote_data->pitch += pitch_offset;

    remote_data->thr += thr_offset;
    remote_data->yaw += yaw_offset;

    // 4. 将值限制在0到1000
    remote_data->roll = (remote_data->roll > 1000) ? 1000 : (remote_data->roll < 0 ? 0 : remote_data->roll);
    remote_data->pitch = (remote_data->pitch > 1000) ? 1000 : (remote_data->pitch < 0 ? 0 : remote_data->pitch);
    remote_data->thr = (remote_data->thr > 1000) ? 1000 : (remote_data->thr < 0 ? 0 : remote_data->thr);
    remote_data->yaw = (remote_data->yaw > 1000) ? 1000 : (remote_data->yaw < 0 ? 0 : remote_data->yaw);
    taskEXIT_CRITICAL();
}

void App_key_scan(Remote_Struct_t *remote_data)
{
    KEY_Type key = Int_ScanButton();

    switch (key)
    {
    case KEY_UP:
        pitch_offset += 10;

        break;
    case KEY_DOWN:
        pitch_offset -= 10;

        break;
    case KEY_LEFT:
        roll_offset -= 10;

        break;
    case KEY_RIGHT:
        roll_offset += 10;

        break;
    case KEY_LEFT_X:
        remote_data->off = 1;
        break;
    case KEY_RIGHT_X:
        remote_data->hold = 1;
        break;
    case KEY_LEFT_X_LONG:
        printf("KEY_LEFT_X_LONG pressed\n");
        break;
    case KEY_RIGHT_X_LONG:
        // 动用偏移量 让油门为0 yaw pitch roll= 500
        roll_offset = 500 - remote_data->roll;
        pitch_offset = 500 - remote_data->pitch;
        yaw_offset = 500 - remote_data->yaw;
        thr_offset = 0 - remote_data->thr;
        break;
    case KEY_NONE:
        // No key pressed
        break;
    default:
        printf("Unknown key pressed\n");
        break;
    }
}

COMM_State App_SendData(Remote_Struct_t *remote_data)
{
    tx_buffer[0] = FRAME_HEAD_1;
    tx_buffer[1] = FRAME_HEAD_2;
    tx_buffer[2] = FRAME_HEAD_3;

    tx_buffer[3] = remote_data->thr >> 8;
    tx_buffer[4] = remote_data->thr & 0xFF;
    tx_buffer[5] = remote_data->pitch >> 8;
    tx_buffer[6] = remote_data->pitch & 0xFF;
    tx_buffer[7] = remote_data->roll >> 8;
    tx_buffer[8] = remote_data->roll & 0xFF;
    tx_buffer[9] = remote_data->yaw >> 8;
    tx_buffer[10] = remote_data->yaw & 0xFF;

    tx_buffer[11] = remote_data->hold;
    tx_buffer[12] = remote_data->off;
    uint32_t x = 0;
    for (uint8_t i = 0; i < 13; i++)
    {
        x += tx_buffer[i];
    }
    tx_buffer[13] = x >> 24;
    tx_buffer[14] = (x >> 16) & 0xFF;
    tx_buffer[15] = (x >> 8) & 0xFF;
    tx_buffer[16] = x & 0xFF;

    // 返回值还没处理 应该做一些善后工作
    uint8_t result = Int_SI24R1_TxPacket(tx_buffer);
    if (result == MAX_RT)
    {
        return COMM_ERROR;
    }
    else if (result == TX_DS)
    {
        remote_data->off = 0;
        remote_data->hold = 0;
        return COMM_OK;
    }
}
