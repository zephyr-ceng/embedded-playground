#include "./stm32f10x.h"
#include "./drive/inc/MyDelay.h"

void TIM2_CH3_PWM_Init()
{
    // 1.TIM4的APB1时钟使能 & GPIO的AFIO复用时钟 & GPIO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    // 2.GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3.TIM时基单元配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    /* 首先为向上计数模式，输出的信号为平滑的PWM信号，所以要提高占空比精度，即小PSC,大ARR*/
    TIM_TimeBaseStructure.TIM_Prescaler     = 72 - 1;      // PSC
    TIM_TimeBaseStructure.TIM_Period        = 1000 - 1;         // ARR
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; // 计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  // 设置时钟分割
    // TIM_TimeBaseStructure.TIM_RepetitionCounter = ; // 重复计数器--TIM1 & TIM8才有高级定时器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 4.PWM配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;        // 模式选择
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;    // 极性设置
    TIM_OCInitStructure.TIM_Pulse       = 0;                      // 设置CCR,初始化占空比
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                      // 选用TIM4_CH2 通道
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); // 使能预装载寄存器，用于比较寄存器，防止产生不稳定
    
    // 5.使能中断
    TIM_ARRPreloadConfig(TIM2, ENABLE); // 使能ARR 预装载寄存器，防止分频的频率突然跳变产生毛刺
    TIM_Cmd(TIM2, ENABLE); // 使能TIM2中断
}

/**
 * @brief  占空比比值设定
 * @param  Compare2 占空比值
 * @retval NULL
 * */

void TIM2_CH3_PWM_SetCompare3(uint16_t Compare2)
{
    TIM_SetCompare3(TIM2, Compare2); // 设置CH3占空比 对应Compare3
}

/**
* @brief  呼吸灯效果
* @param  NULL
* @retval NULL
* */
void breathing_LED(void)
{
    uint16_t i;
    for (i = 0; i < 1000; i++) {
        TIM2_CH3_PWM_SetCompare3(i);
        Delay_ms(1);
    }

    for (i = 0; i < 1000; i++) {
        TIM2_CH3_PWM_SetCompare3(1000 - i);
        Delay_ms(1);
    }
}