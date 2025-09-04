#ifndef __NVIC_H__
#define __NVIC_H__
#include "stm32f10x.h"

#define NVIC_PRIQRITY_GROUP NVIC_PriorityGroup_2
// 默认优先级设置
#define DEFAULT_PREEMPTION_PRIORITY  1
#define DEFAULT_SUB_PRIORITY         0

typedef enum {
    IRQ_USART1,
    IRQ_USART2,
    IRQ_USART3,
    IRQ_TIM2,
    IRQ_TIM3,
    IRQ_TIM4,
    IRQ_EXTI0,
    IRQ_EXTI1,
    IRQ_ADC1_2,
    IRQ_SPI1,
    IRQ_SPI2,
    IRQ_I2C1_EV,
    IRQ_I2C1_ER,
    IRQ_MAX // 用于边界检查
} IRQ_Channel_t;


static const IRQn_Type IRQ_Channel_Mapping[IRQ_MAX] = {
    [IRQ_USART1]    = USART1_IRQn,
    [IRQ_USART2]    = USART2_IRQn,
    [IRQ_USART3]    = USART3_IRQn,
    [IRQ_TIM2]      = TIM2_IRQn,
    [IRQ_TIM3]      = TIM3_IRQn,
    [IRQ_TIM4]      = TIM4_IRQn,
    [IRQ_EXTI0]     = EXTI0_IRQn,
    [IRQ_EXTI1]     = EXTI1_IRQn,
    [IRQ_ADC1_2]    = ADC1_2_IRQn,
    [IRQ_SPI1]      = SPI1_IRQn,
    [IRQ_SPI2]      = SPI2_IRQn,
    [IRQ_I2C1_EV]   = I2C1_EV_IRQn,
    [IRQ_I2C1_ER]   = I2C1_ER_IRQn
};


static const uint8_t default_Priority[IRQ_MAX][2] = {
    [IRQ_USART1]    = {1, 0},
    [IRQ_USART2]    = {1, 0},
    [IRQ_USART3]    = {1, 0},
    [IRQ_TIM2]      = {1, 0},
    [IRQ_TIM3]      = {1, 0},
    [IRQ_TIM4]      = {1, 0},
    [IRQ_EXTI0]     = {1, 0},
    [IRQ_EXTI1]     = {1, 0},
    [IRQ_ADC1_2]    = {1, 0},
    [IRQ_SPI1]      = {1, 0},
    [IRQ_SPI2]      = {1, 0},
    [IRQ_I2C1_EV]   = {1, 0},
    [IRQ_I2C1_ER]   = {1, 0}
};

void NVIC_Configuration(void);
void NVIC_IRQ_Disable(IRQ_Channel_t irq);
void NVIC_SetIRQ_Priority(IRQ_Channel_t irq, uint8_t PreemptionPriority, uint8_t SubPriority);
void NVIC_SetIRQ_DefaultPriority(IRQ_Channel_t irq);
void NVIC_SystemSafeDelay(uint32_t ms);

#endif // !__NVIC_H__
