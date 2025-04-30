/****************
 *@description: 使用TIM4生成1KHz PWM
 *@author: zephyr
 *@date: 2025-04-29 18:55:24
 *@version: V1.0.0
****************/
#ifndef __TIM4_1KHz_H__
#define __TIM4_1KHz_H__

void TIM4_PWM_Init(uint16_t prescaler, uint16_t period);
void TIM4_PWM_SetCompare2(uint16_t Compare2);

#endif // !__TIM4_1KHz_H__
