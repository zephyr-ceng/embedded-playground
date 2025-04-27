/****************
 *@description: 定时器中断控制Number 每秒加1，即定时器频率为1Hz
 *@author: zephyr
 *@date: 2025-04-27 20:52:02
 *@version: V1.0.0
 ****************/
#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

volatile uint16_t Timer_Count_Number=0;

void Timer_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // 滤波器设置
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; // 向上计数模式
    // 要设置1Hz = 72MHz/[(PSC+1)*(ARR+1)
    TIM_TimeBaseStructure.TIM_Prescaler         = 7200 - 1;  // PSC
    TIM_TimeBaseStructure.TIM_Period            = 10000 - 1; // ARR
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;         // 重复计数器设置
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); // 使能中断

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // NVIC分组

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;      // 从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE; // 使能
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM6,ENABLE);
}

/* 
// Handler函数
void TIM6_IRQHandler()
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        Timer_Count_Number++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
 */
uint16_t Get_Value()
{
    return Timer_Count_Number;
}