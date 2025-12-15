/****************
 *@description: TIM1-ETR EternalClock2 - PA12 循迹模块计数
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Xunji.h"

int main(void)
{
    
    OLED_Init();
    TIM1_ETR_ExternalClock1_Overflow_Config();
    OLED_ShowString(1,1,"Count Value:");
    while (1) {
        OLED_ShowNum(2,1,TIM_GetCounter(TIM1),6);
        Delay_ms(1000);
    }
}