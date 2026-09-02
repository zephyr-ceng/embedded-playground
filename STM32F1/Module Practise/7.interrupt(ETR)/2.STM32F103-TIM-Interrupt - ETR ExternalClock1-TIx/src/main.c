/****************
 *@description: TIM1-TI1 ExternalClock1 - PA8 循迹模块计数
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Timer.h"

int64_t Overflow_Count = 0;
int main(void)
{
    
    OLED_Init();
    Timer_Init();
    OLED_ShowString(1,1,"Overflow:");
    while (1) {
        Overflow_Count = Get_Overflow_value(); // 获取计数器溢出次数
        OLED_ShowNum(2,1,TIM_GetCounter(TIM1),6);
        OLED_ShowNum(3,1,Overflow_Count,6); // 溢出次数
        OLED_ShowString(4,1,"TCNT:");
        OLED_ShowNum(4,6,(Overflow_Count * 10 + TIM_GetCounter(TIM1)),6); // 总脉冲数
        
    }
}