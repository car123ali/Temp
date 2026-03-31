#include "Int_VL53L1X.h"

uint16_t dev = 0x52;
void Int_VL53L1X_Init(void)
{
    uint8_t byteData = 0;
    int8_t status = 0;
    uint16_t wordData = 0;
    uint8_t sensorState = 0;

    // 1. 复位传感器
    HAL_GPIO_WritePin(X_SHUT_GPIO_Port, X_SHUT_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(X_SHUT_GPIO_Port, X_SHUT_Pin, GPIO_PIN_SET);
    HAL_Delay(2);

    /* Those basic I2C read functions can be used to check your own I2C functions */
    status = VL53L1_RdByte(dev, 0x010F, &byteData);
    printf("VL53L1X Model_ID: %X\n", byteData);
    status = VL53L1_RdByte(dev, 0x0110, &byteData);
    printf("VL53L1X Module_Type: %X\n", byteData);
    status = VL53L1_RdWord(dev, 0x010F, &wordData);
    printf("VL53L1X: %X\n", wordData);
    while (sensorState == 0)
    {
        status = VL53L1X_BootState(dev, &sensorState);
        HAL_Delay(2);
    }
    printf("Chip booted\n");

    /* This function must to be called to initialize the sensor with the default setting  */
    status = VL53L1X_SensorInit(dev);
    /* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
    status = VL53L1X_SetDistanceMode(dev, 2);           /* 1=short, 2=long */
    status = VL53L1X_SetTimingBudgetInMs(dev, 100);     /* in ms possible values [20, 50, 100, 200, 500] */
    status = VL53L1X_SetInterMeasurementInMs(dev, 100); /* in ms, IM must be > = TB */

    status = VL53L1X_StartRanging(dev); /* This function has to be called to enable the ranging */
}

void Int_VL53L1X_GetDistance(uint16_t *Distance)
{
    uint8_t dataReady = 0;
    int8_t status = 0;

    status = VL53L1X_CheckForDataReady(dev, &dataReady);
    if (dataReady == 1)
    {
        status = VL53L1X_GetDistance(dev, Distance);
        dataReady = 0;
        status = VL53L1X_ClearInterrupt(dev); /* clear interrupt has to be called to enable next interrupt*/
    }
}
