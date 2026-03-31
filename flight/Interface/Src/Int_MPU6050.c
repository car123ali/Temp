#include "Int_MPU6050.h"

void Int_MPU6050_WriteRegByte(uint8_t reg_address, uint8_t byte)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_DEV_ADDRESS, reg_address, I2C_MEMADD_SIZE_8BIT, &byte, 1, 1000);
}

void Int_MPU6050_WriteRegBytes(uint8_t reg_address, uint8_t *bytes, uint8_t size)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_DEV_ADDRESS, reg_address, I2C_MEMADD_SIZE_8BIT, bytes, size, 3000);
}

uint8_t Int_MPU6050_ReadRegByte(uint8_t reg_address)
{
    uint8_t byte = 0;
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_DEV_ADDRESS, reg_address, I2C_MEMADD_SIZE_8BIT, &byte, 1, 1000);
    return byte;
}

void Int_MPU6050_ReadRegBytes(uint8_t reg_address, uint8_t *buffer, uint8_t size)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_DEV_ADDRESS, reg_address, I2C_MEMADD_SIZE_8BIT, buffer, size, 3000);
}

void Int_MPU6050_Init(void)
{
    // 软复位
    Int_MPU6050_WriteRegByte(MPU6050_REG_PWR_MGMT_1, 0x80);
    HAL_Delay(200);
    // bit6 写0 退出睡眠模式
    Int_MPU6050_WriteRegByte(MPU6050_REG_PWR_MGMT_1, 0x00);
    // 让陀螺仪的满量程为 +-2000°/s
    Int_MPU6050_WriteRegByte(MPU6050_REG_GYRO_CONFIG, 3 << 3);
    // 让加速度计的满量程为 +-2g
    Int_MPU6050_WriteRegByte(MPU6050_REG_ACCEL_CONFIG, 0 << 3);
    // 关闭所有的中断使能 因为我们不用中断
    Int_MPU6050_WriteRegByte(MPU6050_REG_INT_ENABLE, 0x00);
    // 不用主I2C 还有 FIFO也不用
    Int_MPU6050_WriteRegByte(MPU6050_REG_USER_CTRL, 0x00);
    // 低通滤波器配置为1
    Int_MPU6050_WriteRegByte(MPU6050_REG_CONFIG, 0x01);
    // 让陀螺仪的采样变成50
    Int_MPU6050_WriteRegByte(MPU6050_REG_SMPRT_DIV, 1);
    // 使用陀螺仪X轴的机械振动 来充当时钟源
    Int_MPU6050_WriteRegByte(MPU6050_REG_PWR_MGMT_1, 0x01);
    // 使能加速度计和陀螺仪
    Int_MPU6050_WriteRegByte(MPU6050_REG_PWR_MGMT_2, 0x00);

    // 做校准
    Int_MPU6050_Calibration();
}

uint8_t data_buffer[6];
void Int_MPU6050_getG(G_Struct *g)
{
    Int_MPU6050_ReadRegBytes(MPU6050_REG_GYRO_XOUT_H, data_buffer, 6);
    g->x = (data_buffer[0] << 8) | data_buffer[1];
    g->y = (data_buffer[2] << 8) | data_buffer[3];
    g->z = (data_buffer[4] << 8) | data_buffer[5];
}

void Int_MPU6050_getA(A_Struct *a)
{
    Int_MPU6050_ReadRegBytes(MPU6050_REG_ACCEL_XOUT_H, data_buffer, 6);
    a->x = (data_buffer[0] << 8) | data_buffer[1];
    a->y = (data_buffer[2] << 8) | data_buffer[3];
    a->z = (data_buffer[4] << 8) | data_buffer[5];
}

// 校准陀螺仪和加速度计
uint8_t count = 0;
A_G_Calibration_Struct a_g_cal;
void Int_MPU6050_Calibration(void)
{
    G_Struct last_g = {0};
    G_Struct current_g = {0};
    while (count < 200)
    {
        // 读取数据 判断
        Int_MPU6050_getG(&current_g);
        // 判断这个值差异大不大
        if (MY_ABS(current_g.x - last_g.x) > 50 || MY_ABS(current_g.y - last_g.y) > 50 || MY_ABS(current_g.z - last_g.z) > 50)
        {
            count = 0;
        }
        else
        {
            count++;
        }
        last_g.x = current_g.x;
        last_g.y = current_g.y;
        last_g.z = current_g.z;
        // 之所以delay 2ms 是为了每一次读取的都是新数据
        // 初始化时 让MPU6050 500hz  一秒钟输出500次数据
        HAL_Delay(2);
    }
    // 开始校准
    A_Struct current_a = {0};
    for (uint8_t i = 0; i < 100; i++)
    {
        Int_MPU6050_getG(&current_g);
        Int_MPU6050_getA(&current_a);

        a_g_cal.a_x += current_a.x;
        a_g_cal.a_y += current_a.y;
        a_g_cal.a_z += current_a.z;
        a_g_cal.g_x += current_g.x;
        a_g_cal.g_y += current_g.y;
        a_g_cal.g_z += current_g.z;
        HAL_Delay(2);
    }
    a_g_cal.a_x /= 100;
    a_g_cal.a_y /= 100;
    a_g_cal.a_z /= 100;
    a_g_cal.g_x /= 100;
    a_g_cal.g_y /= 100;
    a_g_cal.g_z /= 100;
}

void Int_MPU6050_ReadAG_WitchCalbration(A_G_Struct *ag)
{
    // 读取原始数据
    Int_MPU6050_getA(&ag->a);
    Int_MPU6050_getG(&ag->g);
    // printf("%d,", ag->a.z);
    // 减去偏移量做校准
    ag->a.x -= a_g_cal.a_x;
    ag->a.y -= a_g_cal.a_y;
    ag->a.z -= a_g_cal.a_z - 16384;
    ag->g.x -= a_g_cal.g_x;
    ag->g.y -= a_g_cal.g_y;
    ag->g.z -= a_g_cal.g_z;
    // printf("%d\n", ag->a.z);
}