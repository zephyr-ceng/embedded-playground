/****************
 *@description: 旋转编码器-OLED数字加减
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Ranging.h"

uint16_t Distance;
int main(void)
{

    OLED_Init();
    GPIO_Init_Config();
    TIM3_CH_Config();
    OLED_ShowString(1, 1, "Distance Value:");
    while (1) {
        Distance = Get_distance_value();
        OLED_ShowNum(2, 1, Distance,6);
    }
}