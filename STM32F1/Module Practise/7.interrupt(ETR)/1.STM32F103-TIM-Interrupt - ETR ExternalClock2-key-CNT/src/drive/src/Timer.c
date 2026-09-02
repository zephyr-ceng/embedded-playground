/****************
 *@description: 使用外接按键作为 TIM1 ETR 外部脉冲源
 *@author: zephyr
 *@date: 2025-04-27 20:52:02
 *@version: V1.0.0
 ****************/
#include "stm32f10x.h"

void Timer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // TIM1_ETR 的外部输入固定连接到 PA12。
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

    // 外接按键一端接 PA12，另一端接 GND。
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 将 TIM1 恢复为默认状态，避免之前的时钟源、从模式或计数参数影响本次实验。
    TIM_DeInit(TIM1);

    // 配置 TIM1 的时基单元。
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // 自动重装载值为 65535，计数范围为 0~65535
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 重复计数器为 0，表示每次计数到 ARR 就触发更新事件
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);


    // 使用 ETR 外部时钟模式 2，通过 SMCR.ECE 让 ETR 直接驱动 TIM1 计数器。
    TIM_ETRClockMode2Config(TIM1, TIM_ExtTRGPSC_OFF,
                            TIM_ExtTRGPolarity_Inverted, 0);

    // 设置计数器初值，然后启动 TIM1；这里设置为 65530，便于观察即将溢出的计数过程。
    // 启动后 TIM1 不使用内部时钟自动计数，只响应 PA12/ETR 的有效下降沿。
    TIM_SetCounter(TIM1, 65530);
    TIM_Cmd(TIM1, ENABLE);
}

uint16_t Get_Value(void)
{
    return TIM_GetCounter(TIM1);
}

/*
 * 拓展说明：
 * 1. ETR 是 External Trigger 的缩写。模式 1 和模式 2 都会先经过 ETR 的
 *    极性检测、滤波和预分频，但进入计数器的内部路径不同：
 *    - 模式 1：ETRF -> 触发输入选择器（TS 选择 ETRF，输出 TRGI）-> 从模式控制器（SMS=External1）-> TIM1_CNT；
 *    - 模式 2：ETRF -> ETR 外部时钟输入（SMCR.ECE=1）-> TIM1_CNT，不经过触发输入选择器和从模式控制器。
 *    因此，本例使用模式 2 时，每个有效下降沿直接使 TIM1_CNT 加 1。
 *
 * 2. 如果外部脉冲较快，可以使用 TIM_ExtTRGPSC_DIV2、DIV4 或 DIV8 分频，
 *    也可以设置 ETR 滤波值 0x01~0x0F 来抑制窄脉冲和部分噪声。
 *
 * 3. 机械按键会抖动，一次按下可能产生多个下降沿；实际应用应增加硬件或软件消抖。
 *    当前计数器溢出后会从 0 重新计数，如需更长计数范围，可在更新中断中扩展溢出次数。
 */