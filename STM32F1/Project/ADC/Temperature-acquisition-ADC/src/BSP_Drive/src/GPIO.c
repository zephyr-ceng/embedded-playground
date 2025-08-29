#include "../inc/gpio.h"

/**
* @brief  GPIO 初始化函数
* @param  config 初始化结构体，包含GPIOx-端口分区，GPIO_Pin-端口号
* @retval Null
* */
void GPIO_InitPin(GPIO_Config_t config)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启GPIO时钟
    if (config.GPIOx == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (config.GPIOx == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (config.GPIOx == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    else if (config.GPIOx == GPIOD)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    else if (config.GPIOx == GPIOE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = config.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = config.GPIO_Mode;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(config.GPIOx, &GPIO_InitStructure);
}


/**
* @brief  GPIO 端口设置
* @param  GPIOx: GPIO分区
* @param  GPIO_Pin: 引脚
* @param  state: Enable / Disable
* @retval Null  
* */
void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, bool state)
{
    if (state) {
        GPIO_SetBits(GPIOx, GPIO_Pin);
    } else {
        GPIO_ResetBits(GPIOx, GPIO_Pin);
    }
}

/**
* @brief  GPIO 端口输入检测
* @param  GPIOx: 端口分区
* @param  GPIO_Pin: 端口号   
* @retval TRUE: 低电平
* @retval FALSE: 高电平
* */
bool GPIO_GetPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    return (GPIO_ReadInputData(GPIOx) & GPIO_Pin) != 0;
}
