#include "../inc/ADC.h"



void ADC_InitModule(ADC_Config_t config)
{
    ADC_InitTypeDef ADC_InitStructure;

    // 开启ADC时钟
    if (config.ADCx == ADC1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    else if (config.ADCx == ADC2)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    ADC_InitStructure.ADC_Mode               = config.ADC_Mode;
    ADC_InitStructure.ADC_ScanConvMode       = config.ADC_ScanConvMode;
    ADC_InitStructure.ADC_ContinuousConvMode = config.ADC_ContinuousConvMode;
    ADC_InitStructure.ADC_ExternalTrigConv   = config.ADC_ExternalTrigConv;
    ADC_InitStructure.ADC_DataAlign          = config.ADC_DataAlign;
    ADC_InitStructure.ADC_NbrOfChannel       = config.ADC_NbrOfChannel;

    ADC_Init(config.ADCx, &ADC_InitStructure);

    // 配置各个通道
    for (uint8_t i = 0; i < config.ADC_NbrOfChannel; i++) {
        ADC_RegularChannelConfig(config.ADCx,
                                 config.Channels[i].ADC_Channel,
                                 config.Channels[i].ADC_Rank,
                                 config.Channels[i].ADC_SampleTime);
    }

    // 使能ADC并校准
    ADC_Cmd(config.ADCx, ENABLE);
    ADC_ResetCalibration(config.ADCx);
    while (ADC_GetResetCalibrationStatus(config.ADCx));
    ADC_StartCalibration(config.ADCx);
    while (ADC_GetCalibrationStatus(config.ADCx));
}

void ADC_StartConversion(ADC_TypeDef *ADCx)
{
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

void ADC_StopConversion(ADC_TypeDef *ADCx)
{
    ADC_SoftwareStartConvCmd(ADCx, DISABLE);
}

uint16_t ADC_ReadSingleChannel(ADC_TypeDef *ADCx, uint8_t channel)
{
    ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
    while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADCx);
}