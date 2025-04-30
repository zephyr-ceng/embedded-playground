/****************
 *@description: 旋转编码器-OLED数字加减
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/TIM4_1KHz_PWM.h"

uint16_t num;

int main(void)
{
    
    TIM4_Init();
    while (1) {
        // i 代表CCR占空比变化
        for ( i = 0; i < 100; i++)
        {
            PWM_SetCompare2(i);
            Delay_ms(10);
        }

        for ( i = 0; i < 100; i++)
        {
            TIM4_PWM_SetCompare2(100-i);
            Delay_ms(10);
        }
        
        

    }
}