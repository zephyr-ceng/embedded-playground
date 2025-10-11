/****************
 *@description: LED驱动头文件
 *@author: zephyr
 *@date: 2025-10-11 16:43:31
 *@version: V1.0.0
****************/

#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"

#define LED_PERIPH RCC_APB2Periph_GPIOA
#define LED_PORT GPIOA
#define LED_PIN GPIO_Pin_1
void LED_Init(void);
void LED_Toggle(void);

#endif // !__LED_H__
