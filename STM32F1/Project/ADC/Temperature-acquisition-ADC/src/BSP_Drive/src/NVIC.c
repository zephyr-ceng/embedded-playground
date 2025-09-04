#include "../inc/NVIC.h"

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PRIQRITY_GROUP);
}


void NVIC_IRQ_Disable(IRQ_Channel_t irq){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

        NVIC_Init(&NVIC_InitStructure);
    }
}


/**
* @brief  设置指定中断的优先级
* @param  irq: 中断通道
* @param  preemptionPriority: 抢占优先级
* @param  subPriority: 子优先级
* @retval Null
* */
void NVIC_SetIRQ_Priority(IRQ_Channel_t irq, uint8_t preemptionPriority, uint8_t subPriority){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preemptionPriority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority;

        NVIC_Init(&NVIC_InitStructure);
    }
}


/**
* @brief  设置指定中断为默认优先级
* @param  irq: 中断通道
* @retval Null
* */
void NVIC_SetIRQ_DefaultPriority(IRQ_Channel_t irq){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = default_Priority[irq][0];
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = default_Priority[irq][1];

        NVIC_Init(&NVIC_InitStructure);
    }
}


/**
* @brief  系统安全延时函数
* @param  delay: 延时时间，单位ms
* @retval Null
* */
void NVIC_SystemSafeDelay(uint32_t delay){
    // 利用SysTick计时实现延时
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks_needed = (SystemCoreClock / 1000) * ms;
    
    while ((start_tick - SysTick->VAL) < ticks_needed) {
        __NOP();
    }

}
