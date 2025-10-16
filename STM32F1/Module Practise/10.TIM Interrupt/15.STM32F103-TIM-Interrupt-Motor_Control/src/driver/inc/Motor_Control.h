/****************
 *@description: 直流（DC）电机控制
 *@author: zephyr
 *@date: 2025-10-16 15:57:24
 *@version: V1.0.0
****************/

#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#include "stm32f10x.h"

#define MOTOR_A_PIN1 GPIO_Pin_6
#define MOTOR_A_PIN2 GPIO_Pin_7
#define MOTOR_A_PORT GPIOB
#define MOTOR_A_CLK RCC_APB2Periph_GPIOB
#define MOTOR_A_TIM TIM2
#define MOTOR_A_TIM_CLK RCC_APB1Periph_TIM2

void Motor_Init(void);
void Motor_Set_Speed(uint8_t speed);
void Motor_Direction_Reverse(uint8_t flag);
#endif // __MOTOR_CONTROL_H