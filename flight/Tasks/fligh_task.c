#include "fligh_task.h"

// 飞机状态
PLANE_STATE planeState = PLANE_STATE_IDLE;

// 遥控状态
RC_STATE rcState = RC_STATE_CONNECTED;

A_G_Struct ag;

EulerAngle_Struct eulerAngle;

uint16_t height = 0;

// 定高目标值
uint16_t fix_height = 0;

float static_az = 0;

// 遥控数据
Remote_Struct_t remote_data = {0};

void Power_Task(void *pvParameters);
#define POWER_TASK_NAME "Power_Task"
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 5
TaskHandle_t Power_Task_Handle;
#define POWER_TASK_PERIOD_MS 10000

// LED任务
void LED_Task(void *pvParameters);
#define LED_TASK_NAME "LED_Task"
#define LED_TASK_STACK_SIZE 128
#define LED_TASK_PRIORITY 1
TaskHandle_t LED_Task_Handle;
#define LED_TASK_PERIOD_MS 50

// 马达任务
void Motor_Task(void *pvParameters);
#define MOTOR_TASK_NAME "Motor_Task"
#define MOTOR_TASK_STACK_SIZE 128
#define MOTOR_TASK_PRIORITY 4
TaskHandle_t Motor_Task_Handle;
#define MOTOR_TASK_PERIOD_MS 6

// 通讯任务
void Comm_Task(void *pvParameters);
#define COMM_TASK_NAME "Comm_Task"
#define COMM_TASK_STACK_SIZE 128
#define COMM_TASK_PRIORITY 3
TaskHandle_t Comm_Task_Handle;
#define COMM_TASK_PERIOD_MS 6

void Flight_TaskStart(void)
{

    Int_Motor_Start();

    Int_SI24R1_Init(RX_MODE);

    Int_MPU6050_Init();

    Int_VL53L1X_Init();

    // 创建电源任务
    xTaskCreate(Power_Task, POWER_TASK_NAME, POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &Power_Task_Handle);
    // 创建LED任务
    xTaskCreate(LED_Task, LED_TASK_NAME, LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, &LED_Task_Handle);
    // 创建马达任务
    xTaskCreate(Motor_Task, MOTOR_TASK_NAME, MOTOR_TASK_STACK_SIZE, NULL, MOTOR_TASK_PRIORITY, &Motor_Task_Handle);
    // 创建通讯任务
    xTaskCreate(Comm_Task, COMM_TASK_NAME, COMM_TASK_STACK_SIZE, NULL, COMM_TASK_PRIORITY, &Comm_Task_Handle);

    vTaskStartScheduler();
}

void Power_Task(void *pvParameters)
{

    while (1)
    {
        if (ulTaskNotifyTake(pdTRUE, POWER_TASK_PERIOD_MS) > 0)
        {
            Int_IP5305T_Off();
        }
        else
        {
            Int_IP5305T_KeepActive();
        }
    }
}

void LED_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();
    while (1)
    {
        // 鸡头
        switch (rcState)
        {
        case RC_STATE_DISCONNECTED:
            LED_Off(&led_left_top);
            LED_Off(&led_right_top);
            break;
        case RC_STATE_CONNECTED:
            LED_On(&led_left_top);
            LED_On(&led_right_top);
            break;
        }

        // 鸡屁股
        switch (planeState)
        {
        case PLANE_STATE_IDLE:
            if ((preTick - led_left_bottom.last_toggle_tick) > 1000)
            {
                LED_Blink(&led_left_bottom, preTick);
                LED_Blink(&led_right_bottom, preTick);
            }
            break;
        case PLANE_STATE_NORMAL:
            if ((preTick - led_left_bottom.last_toggle_tick) > 100)
            {
                LED_Blink(&led_left_bottom, preTick);
                LED_Blink(&led_right_bottom, preTick);
            }
            break;
        case PLANE_STATE_HOLD:
            LED_On(&led_left_bottom);
            LED_On(&led_right_bottom);
            break;
        case PLANE_STATE_ERROR:
            LED_Off(&led_left_bottom);
            LED_Off(&led_right_bottom);
            break;
        }

        vTaskDelayUntil(&preTick, LED_TASK_PERIOD_MS);
    }
}

// 新增马达任务
void Motor_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();
    while (1)
    {
        // 1. 先获取的角速度和加速度（滤波后的）
        App_motor_GetAGFilter(&ag);
        // 2. 通过角速度和加速度 做 姿态解算 欧拉角
        APP_motor_GetEulerAngle(&ag, &eulerAngle, MOTOR_TASK_PERIOD_MS / 1000.0);
        // 3. 调用PID进行计算
        App_motor_pid(&remote_data, &ag, &eulerAngle, MOTOR_TASK_PERIOD_MS / 1000.0);

        Int_VL53L1X_GetDistance(&height);

        App_height_pid(fix_height, height, static_az, MOTOR_TASK_PERIOD_MS / 1000.0);

        App_motor_runtime(planeState, &remote_data);
        vTaskDelayUntil(&preTick, MOTOR_TASK_PERIOD_MS);
    }
}

// 新增通讯任务
void Comm_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();
    while (1)
    {
        RECEIVE_STATE result = App_remote_receive_data(&remote_data);
        printf("off:%d\n", remote_data.off);
        App_handlePowerOff(&remote_data, &Power_Task_Handle);

        App_remote_handleRCConnectState(result, &rcState);

        App_remote_handleFlightState(&remote_data, rcState, &planeState);

        vTaskDelayUntil(&preTick, COMM_TASK_PERIOD_MS);
    }
}