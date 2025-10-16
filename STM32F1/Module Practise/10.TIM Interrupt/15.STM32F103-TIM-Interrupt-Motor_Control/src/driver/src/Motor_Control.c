#include "./driver/inc/Motor_Control.h"

// GPIO配置 GPIOB6, GPIOB7 复用推挽输出
void Motor_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(MOTOR_A_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = MOTOR_A_PIN1 | MOTOR_A_PIN2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; // 配置电机控制引脚为复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_A_PORT, &GPIO_InitStructure);
}

// 点击PWM定时器配置，频率为20kHz,占空比初始为0%,防止啸叫
void Motor_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 定时器基础配置
    TIM_OCInitTypeDef TIM_OCInitStructure;         // 定时器输出比较配置
    RCC_APB2PeriphClockCmd(MOTOR_A_TIM_CLK, ENABLE);

    // 基础配置
    TIM_TimeBaseStructure.TIM_Prescaler         = 72 - 1;             // 72MHz / 72 = 1MHz
    TIM_TimeBaseStructure.TIM_Period            = 50 - 1;             // 1MHz / 50 = 20kHz
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;       // 时钟分频因子
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                  // 重复计数器值
    TIM_TimeBaseInit(MOTOR_A_TIM, &TIM_TimeBaseStructure);

    // 输出比较配置
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;        // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse       = 0;                      // 初始占空比为0%
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;    // 高电平有效
    TIM_OC1Init(MOTOR_A_TIM, &TIM_OCInitStructure);               // 配置通道1
    TIM_OC1PreloadConfig(MOTOR_A_TIM, TIM_OCPreload_Enable);      // 使能预装载寄存器，在下一个更新事件时更新

    TIM_Cmd(MOTOR_A_TIM, ENABLE); // 使能定时器
}

void Motor_Init()
{
    Motor_GPIO_Init();
    Motor_TIM_Init();
}

// 点击设置电机A的速度和方向
// speed: 0 ~ 100,值表示转速，
void Motor_Set_Speed(uint8_t speed)
{
    if (speed > 100) speed = 100;         // 限制百分比范围
    uint16_t duty = (uint16_t)(speed * 10); // 转换为0~1000的占空比
    TIM_SetCompare1(TIM1, duty);
}

// 反向
void Motor_Direction_Reverse(uint8_t flag)
{
    if (flag) {
        GPIO_SetBits(MOTOR_A_PORT, MOTOR_A_PIN1);
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_A_PIN2);
    } else {
        GPIO_ResetBits(MOTOR_A_PORT, MOTOR_A_PIN1);
        GPIO_SetBits(MOTOR_A_PORT, MOTOR_A_PIN2);
    }
}