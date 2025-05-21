#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"

uint32_t riseTime      = 0; // 上升沿时间
uint32_t fallTime      = 0; // 下降沿时间
uint16_t captureStatus = 0; // 测量状态
uint32_t distance      = 0;

void GPIO_Init_Config()
{
    // 1.IO初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef Gpio_initStructure;
    Gpio_initStructure.GPIO_Pin   = GPIO_Pin_7;
    Gpio_initStructure.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
    Gpio_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &Gpio_initStructure);

    Gpio_initStructure.GPIO_Pin   = GPIO_Pin_6;
    Gpio_initStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING; // 浮空输入
    Gpio_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &Gpio_initStructure);
}

void TIM3_CH_Config()
{
    // 2.初始化中断
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period    = 65535;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // 整个时钟的分频模式
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    // 3.输入捕获配置 通道1
    TIM_ICInitTypeDef TIM_ICIinitStructure;
    TIM_ICIinitStructure.TIM_Channel     = TIM_Channel_1;            // 通道设置
    TIM_ICIinitStructure.TIM_ICPolarity  = TIM_ICPolarity_Rising;    // 上升沿检测
    TIM_ICIinitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直接模式
    TIM_ICIinitStructure.TIM_ICPrescaler = TIM_CKD_DIV1;             // 输入时钟信号的分频设置
    TIM_ICIinitStructure.TIM_ICFilter    = 0;                        // 滤波器设置
    TIM_ICInit(TIM3, &TIM_ICIinitStructure);

/*     // 3.输入捕获配置 通道2
    TIM_ICIinitStructure.TIM_Channel     = TIM_Channel_2;
    TIM_ICIinitStructure.TIM_ICPolarity  = TIM_ICPolarity_Falling;     // 下降沿
    TIM_ICIinitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI; // 间接模式
    TIM_ICIinitStructure.TIM_ICPrescaler = TIM_CKD_DIV1;               // 分频设置
    TIM_ICIinitStructure.TIM_ICFilter    = 0;                          // 滤波器设置
    TIM_ICInit(TIM3, &TIM_ICIinitStructure); */

    // 4.NVIC初始化
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // // 5.配置输入捕获映射
    // TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1); // TIM3 的IT1为触发源
    // TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);

    // 6.清除TIM3 IT_CC1标志位
    // TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 | TIM_IT_CC2);
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    // 7.使能TIM3定时器
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQnHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_FLAG_CC1) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC1);
        captureStatus++;
        // if (captureStatus == 0) {
        //     riseTime      = TIM_GetCounter(TIM3);
        //     captureStatus = 1;
        // }
    }

/*     if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        if (captureStatus == 1) {
            fallTime = TIM_GetCounter(TIM3);
            if (fallTime > riseTime) {
                // 单位CM
                distance = (fallTime - riseTime) * 0.034 / 2;
            } else {
                distance = (0xffff - riseTime) * 0.034 / 2;
            }

            captureStatus = 2;
        }
    } */
}

// 超声波触发模块
void Trigger_Ultrasonic()
{
    GPIO_SetBits(GPIOA, GPIO_Pin_7); // 发送一个高电平
    // Delay_us(10);
    Delay_ms(500);
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    Delay_ms(500);

    captureStatus = 2;
    // TIM_SetCounter(TIM3, 0);
}

int16_t Get_distance_value()
{
    return distance;
}

int16_t Get_captureStatus(){
    return captureStatus;
}