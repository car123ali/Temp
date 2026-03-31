#include "remote_task.h"

Remote_Struct_t remote_data = {0};

void Power_Task(void *pvParameters);
#define POWER_TASK_NAME "Power_Task"
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
TaskHandle_t Power_Task_Handle;
#define POWER_TASK_PERIOD_MS 10000

void Comm_Task(void *pvParameters);
#define COMM_TASK_NAME "Comm_Task"
#define COMM_TASK_STACK_SIZE 128
#define COMM_TASK_PRIORITY 3
TaskHandle_t Comm_Task_Handle;
#define COMM_TASK_PERIOD_MS 6

void Button_Task(void *pvParameters);
#define BUTTON_TASK_NAME "Button_Task"
#define BUTTON_TASK_STACK_SIZE 128
#define BUTTON_TASK_PRIORITY 1
TaskHandle_t Button_Task_Handle;
#define BUTTON_TASK_PERIOD_MS 15

void Joystick_Task(void *pvParameters);
#define Joystick_TASK_NAME "Joystick_Task"
#define Joystick_TASK_STACK_SIZE 128
#define Joystick_TASK_PRIORITY 2
TaskHandle_t Joystick_Task_Handle;
#define Joystick_TASK_PERIOD_MS 6

void Display_Task(void *pvParameters);
#define Display_TASK_NAME "Display_Task"
#define Display_TASK_STACK_SIZE 128
#define Display_TASK_PRIORITY 2
TaskHandle_t Display_Task_Handle;
#define Display_TASK_PERIOD_MS 20

void Remote_TaskStart(void)
{
    Int_SI24R1_Init(TX_MODE);

    Int_Joystick_Init();

    App_Display_Start();

    // 创建电源任务
    xTaskCreate(Power_Task, POWER_TASK_NAME, POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &Power_Task_Handle);

    // 创建通讯任务
    xTaskCreate(Comm_Task, COMM_TASK_NAME, COMM_TASK_STACK_SIZE, NULL, COMM_TASK_PRIORITY, &Comm_Task_Handle);

    // 创建按键任务
    xTaskCreate(Button_Task, BUTTON_TASK_NAME, BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, &Button_Task_Handle);

    // 创建摇杆任务
    xTaskCreate(Joystick_Task, Joystick_TASK_NAME, Joystick_TASK_STACK_SIZE, NULL, Joystick_TASK_PRIORITY, &Joystick_Task_Handle);
    // 创建显示任务
    // xTaskCreate(Display_Task, Display_TASK_NAME, Display_TASK_STACK_SIZE, NULL, Display_TASK_PRIORITY, &Display_Task_Handle);
    vTaskStartScheduler();
}

void Power_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();
    while (1)
    {
        printf("Power_Task\n");
        vTaskDelayUntil(&preTick, POWER_TASK_PERIOD_MS);
        Int_IP5305T_KeepActive();
    }
}

void Comm_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();

    while (1)
    {
        COMM_State result = App_SendData(&remote_data);

        if (result == COMM_OK)
        {
            printf("send success\n");
        }
        else if (result == COMM_ERROR)
        {
            printf("send fail\n");
        }
        vTaskDelayUntil(&preTick, COMM_TASK_PERIOD_MS);
        // 在这里添加通讯任务的具体逻辑
    }
}

void Button_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();
    while (1)
    {
        App_key_scan(&remote_data);
        vTaskDelayUntil(&preTick, BUTTON_TASK_PERIOD_MS);
    }
}

void Joystick_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();

    while (1)
    {
        App_joystick_data(&remote_data);
        printf("%d,%d,%d,%d\n", remote_data.thr, remote_data.roll, remote_data.yaw, remote_data.pitch);
        vTaskDelayUntil(&preTick, Joystick_TASK_PERIOD_MS);
        // 在这里添加通讯任务的具体逻辑
    }
}

void Display_Task(void *pvParameters)
{
    uint32_t preTick = xTaskGetTickCount();
    while (1)
    {
        App_Display_Show(&remote_data);
        vTaskDelayUntil(&preTick, Display_TASK_PERIOD_MS);
        // 在这里添加通讯任务的具体逻辑
    }
}