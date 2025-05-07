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
    
    TIM2_CH3_PWM_Init();
    while (1) {     
        breathing_LED();
    }
}