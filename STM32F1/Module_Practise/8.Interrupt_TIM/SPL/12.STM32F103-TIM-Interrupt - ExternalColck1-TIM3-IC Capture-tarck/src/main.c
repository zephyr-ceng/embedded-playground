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

uint64_t num;

int main(void)
{
    
    OLED_Init();
    TIM3_CH1_Config();
    OLED_ShowString(1,1,"Count Value:");
    while (1) {
        num = Read_Pulse_Width();
        OLED_ShowNum(2, 2, num, 6);
        OLED_ShowNum(3,2,TIM_GetCounter(TIM3),6);
    }
}