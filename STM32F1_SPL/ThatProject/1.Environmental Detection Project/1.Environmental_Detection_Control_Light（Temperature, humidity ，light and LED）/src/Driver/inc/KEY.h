/****************
 *@description: 按键驱动
 *@author: zephyr
 *@date: 2025-10-11 17:12:12
 *@version: V1.0.0
 ****************/

#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY_PERIPH RCC_APB2Periph_GPIOB
#define KEY_PORT   GPIOB
#define KEY_PIN    GPIO_Pin_0
#define KEY_ON     Bit_RESET
#define KEY_OFF    Bit_SET

void KEY_Init(void);
uint8_t KEY_Scan(void);

#endif // !__KEY_H