/****************
 *@description: GPIO 初始化
 *@brief: 提供所有GPIO 初始化内容
 *@author: zephyr
 *@date: 2025-11-24 16:04:29
 *@version: V1.0.0
 ****************/

#include "gpio.h"

// FN_KEY
void FN_KEY_GPIO_Init()
{
    GPIO_InitTypeDef gpio_initStructure;

    // FN功能按键
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpio_initStructure.GPIO_Pin   = FN_Pin;
    gpio_initStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; // 浮空输入
    gpio_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FN_GPIO_Port, &gpio_initStructure);
}

// 74HC165 的PL/CE
void PL_CE_GPIO_Init()
{
    GPIO_InitTypeDef gpio_initStructure;

    // FN功能按键
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 74HC165按键
    gpio_initStructure.GPIO_Pin   = CE_Pin | PL_Pin;
    gpio_initStructure.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
    gpio_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(&FN_GPIO_Port, &gpio_initStructure);
}