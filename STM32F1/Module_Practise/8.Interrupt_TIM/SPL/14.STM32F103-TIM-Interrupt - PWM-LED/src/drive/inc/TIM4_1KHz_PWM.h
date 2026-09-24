/****************
 *@description: 使用TIM4生成1KHz PWM
 *@author: zephyr
 *@date: 2025-04-29 18:55:24
 *@version: V1.0.0
****************/
#ifndef __TIM4_1KHz_H__
#define __TIM4_1KHz_H__

void TIM2_CH3_PWM_Init();
void TIM2_CH3_PWM_SetCompare3(uint16_t Compare2);
void breathing_LED(void);

#endif // !__TIM4_1KHz_H__
