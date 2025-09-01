#include "../inc/NVIC.h"

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PRIQRITY_GROUP);
}


// TODO: NVIC函数需要删减

void NVIC_EnableIRQ(IRQ_Channel_t irq){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_GetPriority(IRQ_Channel_Mapping[irq]) >> 4; // 获取当前优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_GetPriority(IRQ_Channel_Mapping[irq]) & 0x0F; // 获取当前子优先级

        NVIC_Init(&NVIC_InitStructure);
    }
}


void NVIC_DisableIRQ(IRQ_Channel_t irq){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

        NVIC_Init(&NVIC_InitStructure);
    }
}


void NVIC_SetPriority(IRQ_Channel_t irq, uint8_t preemptionPriority, uint8_t subPriority){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preemptionPriority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority;

        NVIC_Init(&NVIC_InitStructure);
    }
}


void NVIC_SetPriority_Default(IRQ_Channel_t irq){
    if(irq < IRQ_MAX){
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = IRQ_Channel_Mapping[irq];
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = default_Priority[irq][0];
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = default_Priority[irq][1];

        NVIC_Init(&NVIC_InitStructure);
    }
}


void NVIC_SystemSafeDelay(uint32_t delay){
    // 利用SysTick计时实现延时
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks_needed = (SystemCoreClock / 1000) * ms;
    
    while ((start_tick - SysTick->VAL) < ticks_needed) {
        __NOP();
    }

}
