#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

/**
* @brief  光敏电阻模拟量初始化
* @param  Null
* @retval NULL
* */
void LDR_Init()
{
    // 定义枚举变量
    GPIO_InitTypeDef gpio_instruct = {0};
    ADC_InitTypeDef adc_instruct   = {0};

    // 时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 端口配置
    gpio_instruct.GPIO_Mode  = GPIO_Mode_AIN;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_9;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_instruct);

    // ADC配置
    adc_instruct.ADC_Mode               = ADC_Mode_Independent;
    adc_instruct.ADC_ScanConvMode       = DISABLE;
    adc_instruct.ADC_ContinuousConvMode = ENABLE;
    adc_instruct.ADC_ExternalTrigConv   = ADC_ExternalTrigInjecConv_None;
    adc_instruct.ADC_DataAlign          = ADC_DataAlign_Right;
    adc_instruct.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc_instruct);

    // ADC通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    // ADC使能
    ADC_Cmd(ADC1, ENABLE);

    // 6. 校准 ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    // 7. 开始 ADC 转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
* @brief  光敏电阻模拟量读取
* @param  Null
* @retval 模拟量转换的数字量
* */
uint16_t Read_Light_Sensor(void) {
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); // 等待转换完成
    return ADC_GetConversionValue(ADC1); // 读取 ADC 值
}

/**
* @brief  光敏电阻的数字量初始化
* @param  Null
* @retval Null
* */
void LDR_Digital_Init()
{
    // 端口初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    GPIO_InitTypeDef gpio_instruct = {0};
    
    // 端口配置
    gpio_instruct.GPIO_Mode = GPIO_Mode_IPU;
    gpio_instruct.GPIO_Pin = GPIO_Pin_9;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio_instruct);
}
/**
* @brief  光敏电阻数字量读取
* @param  Null
* @retval 数字量的值
* */
uint8_t Read_Light_Sensor_Digital(){
    return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9);
}
