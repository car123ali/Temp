#include "Int_Joystick.h"

uint16_t adc_dma_buffer[4];

void Int_Joystick_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, 4);
}

void Int_Joystick_Scan(Remote_Struct_t *remote_data)
{
    remote_data->thr = adc_dma_buffer[0];
    remote_data->yaw = adc_dma_buffer[3];
    remote_data->pitch = adc_dma_buffer[1];
    remote_data->roll = adc_dma_buffer[2];
}