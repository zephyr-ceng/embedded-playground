#include "stm32f10x.h"
#include "./drive/MyDelay.h"

// 初始化板载按键 SW1，PA0/WKUP 为外部下拉输入
void Key1_Init()
{
    GPIO_InitTypeDef gpio_instruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio_instruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_0;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio_instruct);
}

// 初始化板载按键 SW2，PC13 为外部下拉输入
void Key2_Init()
{
    GPIO_InitTypeDef gpio_instruct = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio_instruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio_instruct.GPIO_Pin   = GPIO_Pin_13;
    gpio_instruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&gpio_instruct);
}

// 扫描按键并返回按键编号：0 表示无按键，1 表示 SW1，2 表示 SW2
uint8_t Key_Scan(){
    uint8_t keyNum = 0;

    // PA0 按下时接通 3V3，读取为高电平
    if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==1)
    {
        // 延时消除机械抖动，并再次确认按键确实按下
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==1)
        {
            // 等待按键释放，确保一次按下只产生一次按键事件
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==1);
            Delay_ms(20);
            keyNum = 1;
        }
    }
    // PA0 未按下时检查 PC13 按键
    else if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==1)
    {
        // 延时消除机械抖动，并再次确认按键确实按下
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==1)
        {
            // 等待按键释放，确保一次按下只产生一次按键事件
            while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==1);
            Delay_ms(20);
            keyNum = 2;
        }
    }
    return keyNum;
}