#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"


void TIM1_ETR_ExternalClock1_Overflow_Config()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    // 定时器基础配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period            = 65534;        // ARR
    TIM_TimeBaseStructure.TIM_Prescaler         = 0;            // PSC
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1; // 滤波的分频， 主要用于输入捕获和编码器接口
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_ETRConfig(TIM1, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);
    // 选择输入型号和模式
    TIM_SelectInputTrigger(TIM1, TIM_TS_ETRF);      // 选择TIM1-ETR信号为触发信号
    
    // TODO: 暂时不能观察到其他模式现象，以后改用串口来显示
    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated); // 触发模式为重置，效果为“每次上升沿重复触发计数”

    TIM_ClearFlag(TIM1, TIM_IT_Update); // 恢复上电进入的中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    // 7. 启动定时器
    TIM_Cmd(TIM1, ENABLE);
}