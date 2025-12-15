#include "./Module/inc/ADC.h"

#define ADC_RCC_APB2Periph_GPIO RCC_APB2Periph_GPIOA
#define ADC_GPIO_PORT GPIO_PortSourceGPIOA
#define ADC_IN GPIO_Pin_5
#define ADC_Channel ADC_Channel_2
/********************************************** GPIO初始化 **********************************************/
void ADC_GPIO_Configuration(void){
    GPIO_InitTypeDef Gpio_InitStructure;
    RCC_APB2PeriphClockCmd(ADC_RCC_APB2Periph_GPIO,ENABLE);
    Gpio_InitStructure.GPIO_Pin = ADC_IN;
    Gpio_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    Gpio_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&Gpio_InitStructure);
}
/********************************************** ADC模块初始化 **********************************************/
void ADC_Configuration(void){
    ADC_InitTypeDef ADC_InitStructure;
    // 时钟使能及分频
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 

    ADC_DeInit(ADC1); // 复位ADC1
    // reg（regular）为规则通道  in(injection)为注入通道 
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式 单ADC使用，simultaneous 同步采样
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 单次转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; //  规则转换通道数目
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发 可选择定时器触发等
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1,ENABLE);
    
    // 执行复位校准和ADC校准,必须等待校准结束
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1,ENABLE); // 启动ADC软件触发
}

uint16_t ADC_GetValue(){
    
    ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_239Cycles5); // 规则通道采样周期239.5
    while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

uint16_t ADC_GetAverage(uint8_t count){
    uint32_t temp = 0;
    uint8_t t;
    for ( t = 0; t < count; t++)
    {
        temp+=ADC_GetValue();
        Delay_ms(5); // 没5ms取值一次
    }
    return temp/count;
}
/********************************************** 初始化功能 **********************************************/
void ADC_InitModule(){
    ADC_GPIO_Configuration();
    ADC_Configuration();
}

// 仅计算外部ADC通道采集电压值
float ADC_GetVoltage(){
    uint16_t temp = ADC_GetValue(ADC1);
    return ((float)temp) / 4095.0f * 3.3;
}
