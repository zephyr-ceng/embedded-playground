#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
// ADC配置结构体
typedef struct {
    ADC_TypeDef* ADCx;             // ADC模块
    uint8_t ADC_Channel;           // ADC通道
    uint8_t ADC_SampleTime;        // 采样时间
    uint8_t ADC_Rank;              // 转换序列排名
} ADC_ChannelConfig_t;

void ADC_InitModule(ADC_ChannelConfig_t config);
void ADC_StartConversion(ADC_TypeDef* ADCx);
void ADC_StopConversion(ADC_TypeDef* ADCx);
uint16_t ADC_ReadSingleChannel(ADC_TypeDef* ADCx, uint8_t channel);

#endif // !__ADC_H__