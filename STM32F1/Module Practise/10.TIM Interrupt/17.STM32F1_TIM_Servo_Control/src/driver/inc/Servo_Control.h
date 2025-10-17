#ifndef __SERVO_CONTROL_H
#define __SERVO_CONTROL_H

#include "stm32f10x.h"
#include "./driver/inc/MyDelay.h"

// 使用的是TIM1 CH1 通道，对应引脚 PA8
#define SERVO_PORT GPIOA
#define SERVO_PIN GPIO_Pin_8
#define SERVO_CLK RCC_APB2Periph_GPIOA
#define SERVO_TIM TIM1
#define SERVO_TIM_CLK RCC_APB2Periph_TIM1
#define SERVO_TIM_CHANNEL 

void SERVO_Init();
void Servo_SetAngle( uint8_t angle);

#endif // !__SERVO_CONTROL_H