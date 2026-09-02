/****************
 *@description: TIM1 ETR 外部时钟模式1，统计循迹模块脉冲
 *@author: zephyr
 *@date: 2025-04-27 20:52:02
 *@version: V1.0.0
 ****************/
#include "stm32f10x.h"

static volatile uint32_t Timer_Overflow_Count = 0;

void Timer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE); // 使能 GPIOA 和 TIM1 时钟

    // GPIOA Pin 12 配置为浮空输入，用于接收外部时钟信号
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // TIM1 配置为外部时钟模式1，计数器上升沿计数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;       // 设置时钟分频为 1，即不分频
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; // 设置计数器为向上计数模式
    TIM_TimeBaseStructure.TIM_Prescaler         = 0;                  // 设置预分频器为 0，即不分频，计数器直接使用外部时钟信号
    TIM_TimeBaseStructure.TIM_Period            = 10;                 // 设置自动重装载寄存器的值为 10，计数器溢出时产生更新事件
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                  // 设置重复计数器为 0，即不使用重复计数功能
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // 配置 TIM1 的外部触发输入为 ETR，使用外部时钟模式1, ETR 分频关闭，极性为非反相(上升沿有效),，ETR 滤波值为 0x0F
    TIM_ETRClockMode1Config(
        TIM1,
        TIM_ExtTRGPSC_OFF,
        TIM_ExtTRGPolarity_NonInverted,
        0x0F);

    // 配置 NVIC 中断优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    Timer_Overflow_Count = 0;
    TIM_SetCounter(TIM1, 0);                       // 将计数器清零
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);      // 清除更新中断标志
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // 使能更新中断
    TIM_Cmd(TIM1, ENABLE);                     // 使能 TIM1
}

// TIM1 更新中断服务函数
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        Timer_Overflow_Count++; // 每11个脉冲溢出一次，计数器溢出次数加1
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

// 获取计数器溢出次数
int64_t Get_Overflow_value(void)
{
    return (int64_t) Timer_Overflow_Count; // 返回计数器溢出次数
}


/*
    ETR 分频可选：TIM_ExtTRGPSC_OFF、TIM_ExtTRGPSC_DIV2、
    TIM_ExtTRGPSC_DIV4、TIM_ExtTRGPSC_DIV8。当前关闭 ETR 分频，
    即每个有效 ETR 脉冲都参与计数；例如改为 DIV2 后每 2 个脉冲计数 1 次。
    最后一个参数是 ETR 数字滤波值，范围为 0x00~0x0F：
    0x00 不滤波，数值越大滤波越强，但也要求输入信号保持更长时间。
    循迹模块存在抖动时可保留 0x0F；脉冲较窄或频率较高时应适当降低该值。
    即：如果要过滤掉一些抖动脉冲，可以适当的增加滤波值，但如果脉冲频率较高或脉冲宽度较窄，则应适当降低滤波值。(同按键消抖原理)
    具体滤波值计算公式：滤波时间 = (2^N) / fck_int，其中 N 为滤波值，fck_int 为内部时钟频率。

*/