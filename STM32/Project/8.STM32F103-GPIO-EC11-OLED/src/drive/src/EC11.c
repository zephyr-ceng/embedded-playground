#include "stm32f10x.h"

extern int16_t Count;

/**
 * @brief  A_PB0 & B_PB1 中断配置
 * @param  Null
 * @retval Null
 * */
void EC11_Init(void)
{
    // 1.开启GPIO和AFIO(复用功能)时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // 2.GPIO配置
    GPIO_InitTypeDef gpio_instruct = {0};
    gpio_instruct.GPIO_Mode        = GPIO_Mode_IPD; // 下拉输入
    gpio_instruct.GPIO_Pin         = GPIO_Pin_0 | GPIO_Pin_1;
    gpio_instruct.GPIO_Speed       = GPIO_Speed_50MHz; // 并未使用
    GPIO_Init(GPIOA, &gpio_instruct);

    // 3.AFIO配置
    GPIO_EXTILineConfig(GPIOB, GPIO_PinSource0);

    // 4.EXTI配置
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    // 触发方式 rising 上升沿 falling 下降沿  rising_falling 双边沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // NVIC 中断优先级配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  中断处理函数
 * @param  Null
 * @retval Null
 * */
void EXTI0_IRQHandler(void)
{
    uint16_t currentA = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
    uint16_t currentB = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
    // 中断检测 set 进入中断 reset 退出中断
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
        // currentA 上升沿时 currentB是高电平 为逆时针
        // currentA 上升沿时 currentB是低电平 为逆时针
        if (currentA == 1) {
            if (currentB == currentA)
                Count--;
        } else {
            Count++;
        }
        // TODO: 清除状态
        if (Count > 99) count = 0;
        if (Count < 0) Count = 99;
    }
    EXTI_ClearITPendingBit(EXTI_Line0); // 清除中断标志
}

/**
 * @brief  获取数字
 * @param  Null
 * @retval Null
 * */
int16_t Get_Count(void)
{
    uint16_t temp = Count;
    Count = 0;  
    return temp;
}