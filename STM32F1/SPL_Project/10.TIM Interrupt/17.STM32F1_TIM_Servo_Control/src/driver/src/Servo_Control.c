#include "./driver/inc/Servo_Control.h"

// 脉宽计算
static uint16_t Servo_CalcPulse(uint8_t angle){
    if(angle > 180) angle =180;
    return 500 + (uint16_t)angle * 2000 / 180;
}

// 舵机控制引脚
void SERVO_Init(){
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(SERVO_CLK,ENABLE);
    GPIO_InitStructure.GPIO_Pin = SERVO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推挽复用输出
    GPIO_Init(SERVO_PORT,&GPIO_InitStructure);


    // TIM定时器配置 50HZ
    RCC_APB1PeriphClockCmd(SERVO_TIM_CLK,ENABLE);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1; // 1MHz，1us计数 更精细、平滑
    TIM_TimeBaseInitStructure.TIM_Period = 20000 -1; // 50Hz
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseInit(SERVO_TIM,&TIM_TimeBaseInitStructure);

    // PWM配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 初始化舵机位置
    uint16_t pulse = Servo_CalcPulse(90);
    TIM_OCInitStructure.TIM_Pulse = pulse;
    TIM_OC1Init(SERVO_TIM,&TIM_OCInitStructure);
    
    // 预装载值设置
    TIM_OC1PreloadConfig(SERVO_TIM,TIM_OCPreload_Enable); // CCR 预装载 → 改变角度时平滑更新脉宽
    TIM_ARRPreloadConfig(SERVO_TIM, ENABLE); // ARR 预装载 → 保证周期稳定
    TIM_Cmd(SERVO_TIM, ENABLE);
}


void Servo_SetAngle( uint8_t angle)
{
    uint16_t pulse = Servo_CalcPulse(angle);
    TIM_SetCompare1(SERVO_TIM,pulse);
}