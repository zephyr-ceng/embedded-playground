#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

int32_t Overflow_value = 0; // 溢出计数器

/* void TIM1_ETR_ExternalClock2_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 定时器基础配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = 10;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 滤波的分频， 主要用于输入捕获和编码器接口
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // 配置 ETR 为外部时钟2触发源，边沿触发
    TIM_ETRClockMode2Config(
        TIM1,
        TIM_ExtTRGPSC_OFF,              // 不分频
        TIM_ExtTRGPolarity_NonInverted, // 上升沿触发（可改为 Inverted）
        0x0F                            // 滤波器（最大值，抗抖动）
    );

    // TIM_SetCounter(TIM1, 0);
    TIM_Cmd(TIM1, ENABLE);
} */

void TIM1_ETR_ExternalClock2_Overflow_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    // 定时器基础配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = 9;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 滤波的分频， 主要用于输入捕获和编码器接口
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // 配置 ETR 为外部时钟2触发源，边沿触发
    TIM_ETRClockMode2Config(
        TIM1,
        TIM_ExtTRGPSC_OFF,              // 不分频
        TIM_ExtTRGPolarity_NonInverted, // 上升沿触发（可改为 Inverted）
        0x0F                            // 滤波器（最大值，抗抖动）
    );

    // 6. 配置NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // TIM_SetCounter(TIM1,0);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
    
    // 7. 启动定时器
    TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        Overflow_value++;                            // 溢出计数器加1
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除中断标志
    }

}

int64_t Get_Overflow_value(void)
{
    // TODO:计数会漏掉一次
    return Overflow_value*2;
}