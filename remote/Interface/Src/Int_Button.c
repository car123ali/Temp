#include "Int_Button.h"

KEY_Type Int_ScanButton(void)
{
    // 1.先去读取一个按键的电平
    // 2.如果是低电平 说明按键按下
    // 3. 软件消抖
    // 4. 按键抬起之后再返回这个按键的结果
    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_SET)
                {
                    return KEY_UP;
                }
            }
        }
    }

    if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_SET)
                {
                    return KEY_DOWN;
                }
            }
        }
    }

    if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_SET)
                {
                    return KEY_LEFT;
                }
            }
        }
    }
    if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_SET)
                {
                    return KEY_RIGHT;
                }
            }
        }
    }
    if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
    {
        uint32_t start_tick = xTaskGetTickCount();
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_SET)
                {
                    uint32_t end_tick = xTaskGetTickCount();
                    if (end_tick - start_tick > 1000)
                    {
                        return KEY_LEFT_X_LONG;
                    }
                    else
                    {
                        return KEY_LEFT_X;
                    }
                }
            }
        }
    }

    if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
    {
        uint32_t start_tick = xTaskGetTickCount();
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_SET)
                {
                    uint32_t end_tick = xTaskGetTickCount();
                    if (end_tick - start_tick > 1000)
                    {
                        return KEY_RIGHT_X_LONG;
                    }
                    else
                    {
                        return KEY_RIGHT_X;
                    }
                }
            }
        }
    }
    return KEY_NONE;
}