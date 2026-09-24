#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"


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
