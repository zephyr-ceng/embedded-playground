/****************
 *@description: ADC 数据采集
 *@author: zephyr
 *@date: 2025-09-16 15:02:27
 *@version: V1.0.0
****************/
#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"
#include "./Module/inc/MyDelay.h"
#include "stdio.h"

void ADC_InitModule();
uint16_t ADC_GetValue_Reg();
uint16_t ADC_GetValue_Inject();
uint16_t ADC_GetAverage(uint8_t count);
float ADC_GetVoltage();
#endif // !__ADC_H__