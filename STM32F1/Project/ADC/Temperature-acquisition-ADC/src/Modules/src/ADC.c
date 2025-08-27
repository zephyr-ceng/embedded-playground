/****************
 *@description: ADC通用配置文件
 *@author: zephyr
 *@date: 2025-08-27 17:55:55
 *@version: V1.0.0
 ****************/

#include "../inc/ADC.h"
#include "../inc/MyDelay.h"

#define ADC_Channel_Num 2 // 温度+湿度 两个通道

uint16_t ADC_ConvertedValue[ADC_Channel_Num]; // DMA存储区

void gpio_Config(GPIO_TypeDef *GPIO_Pin_x)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN; // 模拟输入
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_x;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
}

void ADC_Config(ADC_TypeDef *ADCx)
{
    if (ADCx == ADC1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    }
    if (ADCx = ADC2) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    }
    if (ADCx = ADC3) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    }

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent; // 独立工作模式
    ADC_InitStructure.ADC_ScanConvMode       = ENABLE;               // 扫描模式，支持多通道
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;               // 连续转换
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 2; // 两个通道
    ADC_Init(ADC1, &ADC_InitStructure);

    // 规则通道配置
    ADC_RegularChannelConfig(ADCx, ADC_Channel_0,1,ADC_SampleTime_55Cycles5); // 温度
    ADC_RegularChannelConfig(ADCx, ADC_Channel_1,2,ADC_SampleTime_55Cycles5); // 湿度

    ADC_Cmd(ADCx, ENABLE);

    // ADC 校准
    ADC_ResetCalibration(ADCx);
    while (ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);
    while (ADC_GetCalibrationStatus(ADCx));

    // 开始软件触发
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

void DMA_Config(ADC_TypeDef *ADCx)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADCx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = ADC_Channel_Num;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular; // 循环模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_ITConfig(DMA1_Channel1,DMA1_IT_TC1,ENABLE); // DMA 事件监听

}

void ADC_interrupt(){


    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void DMA1_Channel1_IRQHandler(void){
    if (DMA_GetFlagStatus(DMA1_IT_TC1) != RESET)
    {
        DMA_ClearFlag(DMA1_IT_TC1);
        
        
    }
    
}

/**
* @brief  ADC 初始化
* @param  ADCx: ADC通道选择
* @param  GPIO_Pin_x: 输入端口选择
* @retval Null  
* */
void ADC_Config_Init(ADC_TypeDef *ADCx,GPIO_TypeDef *GPIO_Pin_x){
    gpio_Config(GPIO_Pin_x);
    DMA_Config(ADCx);
    ADC_Config(ADCx);
    ADC_interrupt();
}
