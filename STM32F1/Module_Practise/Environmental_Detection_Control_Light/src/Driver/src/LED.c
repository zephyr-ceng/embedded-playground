#include "./Driver/inc/LED.h"


// PWM调光的定时器配置
void PWM_TIM1_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 定时器的时基单元配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseStructure.TIM_Prescaler         = 7200 - 1;           // 预分频器，72MHz/1440=10KHz
    TIM_TimeBaseStructure.TIM_Period            = 1000 - 1;           // 自动重装载寄存器，10KHz/1000=10Hz
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;       // 时钟分频因子
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                  // 重复计数器，仅TIM1和TIM8高级定时器才有, 可以计数0~255次，本次设置为0
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    // PWM模式配置
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;        // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能比较输出
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;    // 输出极性为高
    TIM_OCInitStructure.TIM_Pulse       = 0;                      // 初始占空比为0
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);                      // 配置通道1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);             // 使能预装载寄存器,在下一个更新事件装载到比较寄存器

    // 使能定时器
    TIM_ARRPreloadConfig(TIM1, ENABLE); // 使能自动重装载寄存器
    TIM_Cmd(TIM1, ENABLE);              // 使能定时器
    TIM_CtrlPWMOutputs(TIM1, ENABLE);   // 使能定时器的PWM输出功能
}

// LED初始化
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LED_PERIPH, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED_PIN_GREEN | LED_PIN_BLUE;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; // 复用推挽输出，可输出PWM波形
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    PWM_TIM1_Config(); // 配置PWM定时器
}

// LED状态翻转
void LED_Green_Toggle()
{
    if (GPIO_ReadInputDataBit(LED_PORT, LED_PIN_GREEN) == Bit_SET)
        GPIO_WriteBit(LED_PORT, LED_PIN_GREEN, Bit_RESET);
    else
        GPIO_WriteBit(LED_PORT, LED_PIN_GREEN, Bit_SET);
}



// 设置LED亮度，percent为亮度百分比，范围0~100
void LED_SetDuty(uint8_t percent)
{
    if (percent > 100) percent = 100;         // 限制百分比范围
    uint16_t duty = (uint16_t)(percent * 10); // 转换为0~1000的占空比
    TIM_SetCompare1(TIM1, duty);              // 设置比较寄存器的值
}

// LED 闪缩状态切换
void LED_Blue_Toggle(void)
{
    uint16_t ccr = TIM1->CCR1; // 等价于 TIM1->CCR1
    uint16_t arr = TIM1->ARR;  // 等价于 TIM1->ARR
    float duty   = (float)ccr / (arr + 1) * 100.0f;
    if (duty < 50.0f) {
        LED_SetDuty(100); // 设置为100%亮度
    } else {
        LED_SetDuty(20); // 设置为20%亮度
    }
}

// 蓝色LED呼吸灯效果
void LED_Blue_Breathe(void){
    for (uint8_t i = 0; i <= 100; i++) {
        LED_SetDuty(i); // 渐亮
        Delay_ms(1);
    }
    for (uint8_t i = 100; i > 0; i--) {
        LED_SetDuty(i); // 渐暗
        Delay_ms(1);
    }
}