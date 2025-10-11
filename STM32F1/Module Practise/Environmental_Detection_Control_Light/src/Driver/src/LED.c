#include "./Driver/inc/LED.h"

// LED初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LED_PERIPH,ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT,&GPIO_InitStructure);
}


// LED状态翻转
void LED_Toggle()
{
    if(GPIO_ReadInputDataBit(LED_PORT,LED_PIN) == Bit_SET)
        GPIO_WriteBit(LED_PORT,LED_PIN,Bit_RESET);
    else
        GPIO_WriteBit(LED_PORT,LED_PIN,Bit_SET);
}