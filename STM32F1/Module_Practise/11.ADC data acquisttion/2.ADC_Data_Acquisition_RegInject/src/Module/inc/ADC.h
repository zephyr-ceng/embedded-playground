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
void ADC_GetReg_Illumination(uint8_t count);
void ADC_GetReg_Temperature(uint8_t count);
void ADC_GetInject_Humidity(uint8_t count);
float ADC_GetVoltage_Illumination(uint8_t count);
float ADC_GetVoltage_Temperature(uint8_t count);
float ADC_GetVoltage_Humidity(uint8_t count);
#endif // !__ADC_H__