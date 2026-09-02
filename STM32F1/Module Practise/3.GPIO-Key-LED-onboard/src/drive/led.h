#include "stm32f10x.h"
#include "./drive/MyDelay.h"

void LED_Init()
{
    GPIO_InitTypeDef gpio_instruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio_instruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio_instruct);

    GPIO_SetBits(GPIOA,GPIO_Pin_1);
    GPIO_SetBits(GPIOA,GPIO_Pin_2);
    GPIO_SetBits(GPIOA,GPIO_Pin_3);
}


void Turn_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    if (GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin) == 0) {
        GPIO_SetBits(GPIOx, GPIO_Pin);
    } else {
        GPIO_ResetBits(GPIOx, GPIO_Pin);
    }
}