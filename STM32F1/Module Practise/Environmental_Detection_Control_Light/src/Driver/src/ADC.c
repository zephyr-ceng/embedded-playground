#include "./Driver/inc/ADC.h"

void ADC_Config_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // GPIO初始化
    RCC_APB2PeriphClockCmd(GPIO_ADC_PERIPH,ENABLE); // GPIO时钟使能
    GPIO_DeInit(GPIO_ADC_PORT);
    GPIO_InitStructure.GPIO_Pin = GPIO_ADC_PIN_TEMPRATURE | GPIO_ADC_PIN_LIGHT | GPIO_ADC_PIN_HUMIDITY;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_ADC_PORT,&GPIO_InitStructure);

    // ADC初始化
    RCC_APB1PeriphClockCmd(ADC_PERIPH,ENABLE); // ADC时钟使能
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置ADC时钟为12MHz(72MHz/6=12MHz)  
    ADC_DeInit(ADC);
    RCC_APB2PeriphClockCmd(ADC_PERIPH,ENABLE);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 连续转换模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 右对齐
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_NbrOfChannel = 3; // 转换通道数为1
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; // 扫描模式, 含注入通道和规则通道
    ADC_Init(ADC,&ADC_InitStructure);

    // ADC 规则通道配置
    ADC_RegularChannelConfig(ADC,ADC_CHANNEL_TEMPRATURE,1,ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC,ADC_CHANNEL_LIGHT,2,ADC_SAMPLE_TIME);

    // ADC 注入通道配置
    ADC_InjectedChannelConfig(ADC,ADC_CHANNEL_HUMIDITY,1,ADC_SAMPLE_TIME);
    ADC_InjectedChannelConfig(ADC,ADC_CHANNEL_LIGHT,2,ADC_SAMPLE_TIME);
    ADC_ITConfig(ADC,ADC_IT_JEOC,ENABLE); // 使能注入通道转换结束中断

    // DMA配置
    RCC_AHBPeriphClockCmd(ADC_DMA_PERIPH,ENABLE); // DMA时钟使能
    DMA_DeInit(ADC_DMA_CHANNEL);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC->DR; 
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)ADC_ConvertedValue; // DMA存储器地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC; // 外设作为数据源， DST 为内存到外设
    DMA_InitStructure.DMA_BufferSize         = ADC_CHANNEL_COUNT; // DMA数据宽度，采集3个通道
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; // 外设地址不变
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable; // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte; // 内存数据宽度为8位
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular; // 循环模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High; // 高优先级
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; // 禁用内存到内存的传输
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

    // 使能DMA、ADC
    ADC_Cmd(ADC,ENABLE); // 使能ADC
    DMA_Cmd(ADC_DMA_CHANNEL,ENABLE); // 使能DMA通道
    ADC_DMACmd(ADC,ENABLE); // 使能ADC的DMA功能

    // 校准ADC
    ADC_StartCalibration(ADC);
    while(ADC_GetCalibrationStatus(ADC)); // 等待校准完成
    ADC_ResetCalibration(ADC);
    while(ADC_GetCalibrationStatus(ADC)); // 等待校准完成
    ADC_SoftwareStartConvCmd(ADC,ENABLE); // 软件触发ADC转换
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