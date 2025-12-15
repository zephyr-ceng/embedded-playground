#include "./Module/inc/ADC.h"

#define illumination_GPIO       GPIO_Pin_0 // PC0 光敏电阻
#define ADC_Temperature_GPIO    GPIO_Pin_1 // PC1 温度传感器
#define humidity_GPIO           GPIO_Pin_2 // PC2 湿度传感器

#define illumination_Channel    ADC_Channel_10 // PC0 光敏电阻
#define ADC_Temperature_Channel ADC_Channel_11 // PC1 温度传感器
#define humidity_Channel        ADC_Channel_12 // PC2 湿度传感器

volatile uint16_t ADC_Temperature  = 0; // 规则通道：温度
volatile uint16_t ADC_Humidity     = 0; // 注入通道：湿度
volatile uint16_t ADC_Illumination = 0; // 规则通道：光敏

#define ADC_CHANNEL_COUNT 3
uint16_t ADC_ConvertedValue[ADC_CHANNEL_COUNT]; // 存放结果数组

/********************************************** GPIO初始化 **********************************************/
void ADC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef Gpio_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    Gpio_InitStructure.GPIO_Pin   = illumination_GPIO | ADC_Temperature_GPIO | humidity_GPIO;
    Gpio_InitStructure.GPIO_Mode  = GPIO_Mode_AIN; // 模拟输入
    Gpio_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &Gpio_InitStructure);
}
/********************************************** ADC模块初始化 **********************************************/
void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // ADC配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    ADC_DeInit(ADC1); // 复位ADC1
    ADC_InitStructure.ADC_Mode               = ADC_Mode_RegInjecSimult;   // 同时采样 reg（regular）为规则通道  in(injection)为注入通道
    ADC_InitStructure.ADC_ScanConvMode       = ENABLE;                    // 多规则通道时设置ENABLE，单一规则通道时设置DISABLE，injection通道不受此参数影响
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    // 连续转换
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;       // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel       = 3;                         //  规则转换通道数目
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None; // 软件触发 可选择定时器触发等
    ADC_Init(ADC1, &ADC_InitStructure);

    // ADC规则通道配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5); // 规则通道采样周期239.5
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5); // 规则通道采样周期239.5
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5); // 规则通道采样周期239.5

    // ADC注入通道配置
    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None); // 注入通道软件触发
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);                                  // 使能注入通道中断

    // DMA配置
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)ADC_ConvertedValue;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = ADC_CHANNEL_COUNT;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);


    // 使能ADC和DMA
    ADC_Cmd(ADC1, ENABLE);         // 使能ADC1
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE); // 使能ADC的DMA请求

    // 执行复位校准和ADC校准,必须等待校准结束
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC软件触发规则通道
}

void ADC_InitModule()
{
    ADC_GPIO_Configuration();
    ADC_Configuration();
}

/********************************************** 获取ADC采样值 **********************************************/

// 获取规则通道的光敏采集平均值
void ADC_GetReg_Illumination(uint8_t count)
{
    uint32_t temp = 0;
    for (uint8_t i = 0; i < count; i++) {
        temp += ADC_ConvertedValue[0]; // 累加规则通道的第一个值
        Delay_ms(5);                   // 每5ms取值一次
    }
    ADC_Illumination = (temp / count); // 计算平均值
}

// 获取规则通道的温度采集平均值
void ADC_GetReg_Temperature(uint8_t count)
{
    uint32_t temp = 0;
    for (uint8_t i = 0; i < count; i++) {
        temp += ADC_ConvertedValue[1]; // 累加规则通道的第二个值
        Delay_ms(5);                   // 每5ms取值一次
    }
    ADC_Temperature = (temp / count); // 计算平均值
}

// 获取注入通道的湿度采集平均值
void ADC_GetInject_Humidity(uint8_t count)
{
    uint32_t temp = 0;
    uint8_t t;
    for (t = 0; t < count; t++) {

        ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE); // 启动ADC软件触发注入通道
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC)); // 等待转换结束
        temp += ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        Delay_ms(5); // 每5ms取值一次
    }
    ADC_Humidity = (temp / count); // 计算平均值
}

/********************************************** 初始化功能 **********************************************/


// 规则通道采集光敏电压值
float ADC_GetVoltage_Illumination(uint8_t count)
{
    ADC_GetReg_Illumination(count);
    return ((float)ADC_Illumination) / 4095.0f * 3.3;
}

// 规则通道采集温度电压值
float ADC_GetVoltage_Temperature(uint8_t count)
{
    ADC_GetReg_Temperature(count);
    return ((float)ADC_Illumination) / 4095.0f * 3.3;
}


// 注入通道采集湿度电压值
float ADC_GetVoltage_Humidity(uint8_t count)
{
    ADC_GetInject_Humidity(count);
    return ((float)ADC_Humidity) / 4095.0f * 3.3;
}
