#include "./driver/inc/Ultrasonic_ranging.h"

uint16_t _micros_Overflow = 0;

void Ultrasonic_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruture;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStruture.GPIO_Pin   = TRIG_PIN;
    GPIO_InitStruture.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStruture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_PORT, &GPIO_InitStruture);

    GPIO_InitStruture.GPIO_Pin  = ECHO_PIN;
    GPIO_InitStruture.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
    GPIO_Init(GPIO_PORT, &GPIO_InitStruture);
}

// TIM us 定时器
void TIM2_Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimBaseInitStucture;
    NVIC_InitTypeDef NVIC_InitStucture;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 基础配置
    TIM_TimBaseInitStucture.TIM_Prescaler     = 72 - 1;    // 72MHz/72 = 1MHz,即 1us
    TIM_TimBaseInitStucture.TIM_Period        = 65536 - 1; // 计65535 us 即65.535ms
    TIM_TimBaseInitStucture.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimBaseInitStucture.TIM_CounterMode   = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(TIM2, &TIM_TimBaseInitStucture);

    // 中断溢出
    NVIC_InitStucture.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStucture.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStucture.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStucture.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStucture);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 启动TIM2 更新中断
    TIM_Cmd(TIM2, DISABLE);
}

void Ultrasonic_Ranging_Init()
{
    Ultrasonic_GPIO_Init();
    TIM2_Init();
}

void TIM2_IRQnHandler()
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        _micros_Overflow++; // 溢出次数统计
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

/* void Delay_us(uint32_t us)
{
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
    while (TIM_GetCounter < us);
    TIM_Cmd(TIM2, DISABLE)
}

void Delay_ms(uint16_t ms)
{
    TIM_SetCounter(TIM2, 0);
    TIM_Cmd(TIM2, ENABLE);
    while (ms--)
        Delay_us(1000);
    TIM_Cmd(TIM2, DISABLE)
} */

uint64_t micros()
{
    return (_micros_Overflow << 16) + TIM_GetCounter(TIM2);
}

// 超声波测距
float Ultrasonic_Ranging_GetDistance()
{
    uint16_t time_us = 0;
    // 触发信号
    GPIO_WriteBit(GPIO_PORT, TRIG_PIN, Bit_SET);

    GPIO_WriteBit(GPIO_PORT, TRIG_PIN, Bit_RESET);
    TIM_Cmd(TIM2, ENABLE);
    TIM_SetCounter(TIM2, 0);
    while (GPIO_ReadInputDataBit(GPIO_PORT, ECHO_PIN) != Bit_RESET);
    uint32_t start_Time = micros();

    while (GPIO_ReadInputDataBit(GPIO_PORT, ECHO_PIN) != Bit_RESET);
    uint32_t end_Time = micros();

    TIM_Cmd(TIM2, DISABLE);
    time_us = end_Time - start_Time;
    return (time_us * 0.0343f) / 2.0f; // cm
}