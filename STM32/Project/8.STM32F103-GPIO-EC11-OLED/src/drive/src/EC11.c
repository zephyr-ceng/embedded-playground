#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

uint16_t EC_Count = 0;

void EC11_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // 初始化端口为上拉输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // AFIO配置
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

    // 触发方式--下降沿触发
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0 | EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // NVIC配置优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // NVIC_InitStructure.NVIC_IRQChannel                   = EXTI1_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
    // NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);
}

uint16_t EC_Count_Get()
{
    uint16_t temp;
    temp = EC_Count;
    return temp;
}

// EXTI0 中断
void EXTI0_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            Delay_ms(7);
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
                EC_Count++;
            }else{
                EC_Count--;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/* // EXTI1 中断
void EXTI1_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
            Delay_ms(5);
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
                EC_Count--;
            }
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
} */
