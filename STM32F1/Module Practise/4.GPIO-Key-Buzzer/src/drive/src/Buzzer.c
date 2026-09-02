#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

/**
* @brief  有源蜂鸣器初始化
* @param  NULL
* @retval NULL
* */
void Buzzer_Init()
{
    // 端口初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitTypeDef gpio_instruct = {0};
    
    // 端口配置
    gpio_instruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    gpio_instruct.GPIO_Pin = GPIO_Pin_10;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpio_instruct);
}

/**
* @brief  有源蜂鸣器设置响应时间
* @param  Null
* @retval Null
* */
void Buzzer_Set()
{
    GPIO_SetBits(GPIOB,GPIO_Pin_10); // 设置PB10为高电平，蜂鸣器响
    Delay_ms(100);
    GPIO_ResetBits(GPIOB,GPIO_Pin_10); // 设置PB10为低电平，蜂鸣器停止响
}

