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
    gpio_instruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_instruct.GPIO_Pin = GPIO_Pin_10;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpio_instruct);
}

void Buzzer_Set_ON()
{
    GPIO_SetBits(GPIOB,GPIO_Pin_10);
} 
void Buzzer_Set_OFF()
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}

/**
* @brief  蜂鸣器开和关的时间,单位ms
* @param  number 响的时间，关的时间为1000-number
* @retval 
* */

void Buzzer_Turn(uint16_t number)
{
    if (GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_10) == 0) {
        GPIO_SetBits(GPIOB,GPIO_Pin_10);
        Delay_ms(number);
    } else {
        GPIO_ResetBits(GPIOB,GPIO_Pin_10);
        Delay_ms(1000-number);
    }
}

