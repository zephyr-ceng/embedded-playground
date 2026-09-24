/****************
 *@description: 按键驱动
 *@author: zephyr
 *@date: 2025-10-16 16:31:42
 *@version: V1.0.0
 ****************/

#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "./driver/inc/MyDelay.h"

// 四个按键分别为：开始/停止、加速、减速、反向
#define KEY_POWER          GPIO_Pin_0
#define KEY_SPEED_UP_PIN   GPIO_Pin_1
#define KEY_SPEED_DOWN_PIN GPIO_Pin_2
#define KEY_REVERSE_PIN    GPIO_Pin_3


#define KEY_PORT           GPIOB
#define KEY_CLK            RCC_APB2Periph_GPIOB

extern uint8_t KEY_RUNING; // 按键运行标志

void Key_GPIO_Init(void);
uint8_t Key_Status(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t Key_Scan();

#endif // __KEY_H