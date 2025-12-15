/****************
 *@description: 板载按键检测
 *@author: zephyr
 *@date: 2025-03-30 15:57:39
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/Key_Onboard.h"


/**
* @brief  板载按键初始化(key1 & key2 有效)
* @param  NULL
* @retval NULL
* */
void KEY_Init()
{
    GPIO_InitTypeDef gpio_instruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    // key1
    gpio_instruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_0;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_instruct);

    // key3 Reset
    gpio_instruct.GPIO_Mode = GPIO_Mode_IPD;
    gpio_instruct.GPIO_Pin  = GPIO_Pin_15;
    GPIO_Init(GPIOB, &gpio_instruct);

    // key2
    gpio_instruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_instruct.GPIO_Pin  = GPIO_Pin_13;
    GPIO_Init(GPIOC, &gpio_instruct);
}

/**
* @brief  按键状态检测
* @param  GPIOX: 按键分区，A,B,C
* @param  GPIO_Pin: 具体按键 0~16
* @param  key_PressStatus  KEY_LOW_TRIGGER  = 0,KEY_HIGH_TRIGGER = 1
* @retval KEY_DOWN:1
* @retval KEY_UP:0
* */
uint8_t KEY_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, KEY_TriggerLevel key_PressStatus)
{
    if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == key_PressStatus) {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == key_PressStatus);
        Delay_ms(20);
        return KEY_DOWN;
    } else {
        return KEY_UP;
    }
}

/* uint8_t Key_Scan(){
    uint8_t keyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
    {
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0);
        Delay_ms(20);
        keyNum = 1;
    }
    if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0)
    {
        Delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0);
        Delay_ms(20);
        keyNum = 2;
    }
    return keyNum;
} */
