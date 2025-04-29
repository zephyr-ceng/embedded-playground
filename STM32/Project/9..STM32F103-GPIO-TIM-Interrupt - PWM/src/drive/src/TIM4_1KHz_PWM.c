#include "./stm32f10x.h"

void TIM4_Init(void)
{
    // 时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 设置AFIO复用时钟，在GPIO端口不对应情况下

    // GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = ;
    GPIO_InitStructure.GPIO_Pin   = ;
    GPIO_InitStructure.GPIO_Speed = ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // TIM时基单元配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler         = ; // PSC
    TIM_TimeBaseStructure.TIM_Period            = ; // ARR
    TIM_TimeBaseStructure.TIM_CounterMode       = ; // 计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision     = ; // 设置时钟分割
    TIM_TimeBaseStructure.TIM_RepetitionCounter = ; // 重复计数器

    // PWM配置

    // 使能中断
}

void TIM4_IRQHandler(void){
    if (TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    }
    
}