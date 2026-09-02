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


    TIM_ETRClockMode1Config(TIM1, TIM_ExtTRGPSC_OFF,
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
 * 1. ETR 是 External Trigger 的缩写。在本实验中，ETR 不用于触发一次性动作，
 *    而是通过外部时钟模式 1 直接作为 TIM1 计数器的时钟输入。
 *
 * 2. 当前配置的信号路径为：
 *    PA12 -> ETR 输入 -> 极性检测 -> ETRF -> 外部时钟模式 1 -> TIM1_CNT。
 *    因此每个有效下降沿使 TIM1_CNT 增加 1，OLED 读取的就是这个硬件计数器。
 *
 * 3. 如果外部脉冲频率较高，可以使用 TIM_ExtTRGPSC_DIV2、DIV4 或 DIV8，
 *    让多个外部脉冲合并为一个计数；也可以将 ETR 滤波值设置为 0x01~0x0F，
 *    用于抑制窄脉冲和部分噪声。
 *
 * 4. 机械按键会产生抖动，一次按下可能产生多个下降沿。实际应用中应增加 RC
 *    硬件消抖、施密特触发器，或使用 GPIO/定时器捕获后进行软件消抖。
 *
 * 5. 当前自动重装载值为 0xFFFF，计数达到 65535 后再次收到有效脉冲会回到 0。
 *    如果需要统计更长时间，可以在更新事件中断中扩展溢出次数，组合成 32 位计数。
 */