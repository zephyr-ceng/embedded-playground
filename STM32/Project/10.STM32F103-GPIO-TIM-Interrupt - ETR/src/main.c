/****************
 *@description: 旋转编码器-OLED数字加减
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Xunji.h"

int64_t Count_Value=0;
int main(void)
{
    
    OLED_Init();
    TIM1_ETR_ExternalClock2_Overflow_Config();
    OLED_ShowString(1,1,"Count Value:");
    while (1) {
        Count_Value = Get_Overflow_value();
        OLED_ShowNum(2,1,TIM_GetCounter(TIM1),6);
        OLED_ShowNum(3,1,Count_Value,6);
    }
}