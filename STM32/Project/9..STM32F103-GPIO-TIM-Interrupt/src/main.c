/****************
 *@description: TIM4基础计数器 - 旋转编码器
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Timer.h"

uint16_t num;

int main(void)
{
    
    OLED_Init();
    Timer_Init();
    while (1) {
        num = Get_Value();
        OLED_ShowNum(2, 2, num, 4);
        OLED_ShowNum(3,2,TIM_GetCounter(TIM4),6);
    }
}