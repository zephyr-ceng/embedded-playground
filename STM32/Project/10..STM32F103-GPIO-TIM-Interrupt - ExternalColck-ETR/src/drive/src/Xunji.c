#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

uint16_t Count = 0;

void TIM2_ETR_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATINGGPIO;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 定时器基础配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler     = 72 - 1; // 1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置 ETR 为外部触发源，边沿触发
    TIM_ETRClockMode1Config(
        TIM2,
        TIM_ExtTRGPSC_OFF,               // 不分频
        TIM_ExtTRGPolarity_NonInverted, // 上升沿触发（可改为 Inverted）
        0x0F                             // 滤波器（最大值，抗抖动）
    );


    // NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 设置从模式，ETR触发时计数器启动
    TIM_SelectInputTrigger(TIM2, TIM_TS_ETRF);
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Trigger);

    // 开启更新中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 启动定时器
    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        // TIM_GetCapture1(TIM2);  中断进入时的当前计数器值
        Count++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

// 输入数据捕获
uint16_t Read_Pulse_Width(void)
{
    return Count;
}