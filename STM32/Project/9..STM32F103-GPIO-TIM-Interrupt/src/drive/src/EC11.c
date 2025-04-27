#include "stm32f10x.h" // Device header

volatile int16_t encoderCount;

void encoder_init(void)
{
    // 1.时钟使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 开启RCC时钟

    // 2.GPIO初始化
    GPIO_InitTypeDef GPIO_initstruct;           // 结构体名字GPIO_initstruct
    GPIO_initstruct.GPIO_Mode  = GPIO_Mode_IPU; // 上拉输入
    GPIO_initstruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_initstruct); // 传地址

    // 3.AFIO映射（将GPIO端口和中断关联）
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    // GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

    // 4.中断初始化
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 因为上面是GPIO_Mode_IPU设置为高电平，所以触发中断是下降
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
        // 读取当前A、B相状态
        static uint16_t lastA = 0;
        // static lastB = 0;
        uint8_t currentA = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
        uint8_t currentB = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);

        // 状态机判断方向（需结合A、B相变化）
        if (lastA == 0 && currentA == 1) {            // A上升沿
            currentB == 0 ? encoderCount++ : encoderCount--; // B=0:CW, B=1:CCW
        } else if (lastA == 1 && currentA == 0) {     // A下降沿
            currentB == 1 ? encoderCount++ : encoderCount--; // B=1:CW, B=0:CCW
        }

        lastA = currentA;
        // lastB = currentB;

        EXTI_ClearITPendingBit(EXTI_Line0); // 清除中断标志
    }
}

int16_t encoder_get(void){
    // 中断处配置了上升沿下降沿均会触发两次
    return encoderCount/2; 
}
