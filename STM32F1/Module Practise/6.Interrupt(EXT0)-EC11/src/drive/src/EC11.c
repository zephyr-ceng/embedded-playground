#include "stm32f10x.h" // Device header

volatile int16_t encoderCount;
static uint8_t lastA;

void encoder_init(void)
{
    // 1.时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 开启RCC时钟

    // 2.GPIO初始化
    GPIO_InitTypeDef GPIO_initstruct;
    GPIO_initstruct.GPIO_Mode  = GPIO_Mode_IPU; // 上拉输入
    GPIO_initstruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_initstruct); // 传地址

    // 读取上电后的实际A相状态，避免第一次边沿被误判
    lastA = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);

    // 3.AFIO映射（将GPIO端口和中断关联）
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

    // 4.中断初始化
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 5.NVIC优先级配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 分组方式，整个芯片只能用一种。如放在模块中进行分组，要确保每个模块分组都选的是同一个；或者将这个代码放在主函数的最开始
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// 6.中断服务函数，EXTI0中断服务函数（A相）
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        uint8_t currentA = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
        uint8_t currentB = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);

        if (lastA == 0 && currentA == 1) {
            currentB == 0 ? encoderCount++ : encoderCount--;
        } else if (lastA == 1 && currentA == 0) {
            currentB == 1 ? encoderCount++ : encoderCount--;
        }

        lastA = currentA;
        EXTI_ClearITPendingBit(EXTI_Line0); // 清除中断标志
    }
}

int16_t encoder_get(void)
{
    // A相上升沿和下降沿均触发，每个定位约计数两次
    return encoderCount / 2;
}
