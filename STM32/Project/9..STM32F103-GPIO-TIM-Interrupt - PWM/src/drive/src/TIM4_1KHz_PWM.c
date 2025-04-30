#include "./stm32f10x.h"

/**
* @brief  TIM4_CH2 通道的单路PWM输出
* @param  prescaler 分频值
* @param  prescaler 分辨率
* @retval NULL
* */
void TIM4_CH2_PWM_Init(uint16_t prescaler, uint16_t period)
{
    // 1.TIM4的APB1时钟使能 & GPIO的AFIO复用时钟 & GPIO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE); // 设置AFIO复用时钟，在GPIO端口不对应情况下

    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // TIM时基单元配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler         = prescaler-1; // PSC
    TIM_TimeBaseStructure.TIM_Period            = period-1; // ARR
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; // 计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0; // 设置时钟分割
    // TIM_TimeBaseStructure.TIM_RepetitionCounter = ; // 重复计数器
    TIM_TimeBaseInit(TIM4,TIM_TimeBaseStructure);

    // PWM配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; // 模式选择
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 极性设置
    TIM_OCInitStructure.TIM_Pulse = 0; // 设置CCR,初始化占空比
    TIM_OC2Init(TIM4,&TIM_OCInitStructure); // 选用TIM4_CH2 通道


    // 使能中断
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable); // 使能预装载寄存器
    // TIM_ARRPreloadConfig(TIM4, ENABLE); // 使能ARR 预装载寄存器，防止分频的评率突然跳变产生毛刺
    TIM_Cmd(TIM4,ENABLE); // 使能TIM4中断    
}

void TIM4_CH2_PWM_SetCompare2(uint16_t Compare2){
    TIM_SetCompare2(TIM2,Compare2); // 设置占空比
}


void 