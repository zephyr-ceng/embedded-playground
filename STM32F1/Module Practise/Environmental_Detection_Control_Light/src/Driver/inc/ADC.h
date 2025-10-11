/****************
 *@description: ADC驱动程序
 *@author: zephyr
 *@date: 2025-10-11 17:32:01
 *@version: V1.0.0
 ****************/
#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"
#include "./Driver/inc/MyDelay.h"
#include "stdio.h"

// ADC引脚配置
#define GPIO_ADC_PERIPH         RCC_APB2Periph_GPIOC // GPIO外设时钟
#define GPIO_ADC_PORT           GPIOC                // GPIO端口
#define GPIO_ADC_PIN_TEMPRATURE GPIO_Pin_1           // 温度传感器引脚
#define GPIO_ADC_PIN_LIGHT      GPIO_Pin_2           // 光照传感器引脚
#define GPIO_ADC_PIN_HUMIDITY   GPIO_Pin_3           // 湿度传感器引脚

// ADC参数配置
#define ADC_PERIPH             RCC_APB2Periph_ADC1      // ADC外设时钟
#define ADC                    ADC1                     // 使用的ADC模块
#define ADC_CHANNEL_TEMPRATURE ADC_Channel_11           // 温度传感器ADC通道
#define ADC_CHANNEL_LIGHT      ADC_Channel_12           // 光照传感器ADC通道
#define ADC_CHANNEL_HUMIDITY   ADC_Channel_13           // 湿度传感器ADC通道
#define ADC_SAMPLE_TIME        ADC_SampleTime_55Cycles5 // ADC采样时间

// DMA参数配置
#define ADC_DMA_PERIPH    RCC_AHBPeriph_DMA1 // DMA外设时钟
#define ADC_DMA_CHANNEL   DMA1_Channel1      // 使用的DMA通道

#define ADC_CHANNEL_COUNT 3              // ADC通道数量
volatile uint16_t ADC_ConvertedValue[3]; // ADC转换结果缓存
volatile uint16_t ADC_Illumination;    // 光照强度采集值
volatile uint16_t ADC_Temperature;     // 温度采集值
volatile uint16_t ADC_Humidity;        // 湿度采集值

void ADC_Config_Init(void);
float ADC_GetVoltage_Illumination(uint8_t count);
float ADC_GetVoltage_Temperature(uint8_t count);
float ADC_GetVoltage_Humidity(uint8_t count);


#endif // !__ADC_H
