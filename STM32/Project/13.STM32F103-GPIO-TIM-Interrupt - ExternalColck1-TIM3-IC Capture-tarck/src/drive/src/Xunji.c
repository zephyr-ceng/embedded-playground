/*
在基础定时器上面添加输入捕获，并替换相关的中断类型
*/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

uint64_t Count    = 65534; // 防止溢出
void TIM3_CH1_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 定时器基础配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler     = 72-1; // 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 输入捕获配置
    //  TIM_ICPrescaler和 Filter 主要作用是用于输入的数字滤波，防止边沿检测的跳变
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising; // 检测上升沿
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter    = 15;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    // NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 允许中断
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_FLAG_CC1) != RESET) {
        // TIM_GetCapture1(TIM3);  中断进入时的当前计数器值
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC1);
        Count++;
    }
}

// 输入数据捕获
uint64_t Read_Pulse_Width(void)
{

    return Count;
}