#include "stm32f10x.h"
#include "./drive/MyDelay.h"


void Key1_Init()
{
    GPIO_InitTypeDef gpio_instruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpio_instruct.GPIO_Mode  = GPIO_Mode_IPU;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_10;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpio_instruct);
}

void Key2_Init()
{
    GPIO_InitTypeDef gpio_instruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpio_instruct.GPIO_Mode  = GPIO_Mode_IPU;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_12;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpio_instruct);
}

uint8_t Key_Scan(){
    uint8_t keyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==0)
    {
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)==0);
        Delay_ms(20);
        keyNum = 1;
    }
    if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0)
    {
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)==0);
        Delay_ms(20);
        keyNum = 2;
    }
    return keyNum;
}