/****************
 *@description: 主程序
 *@author: zephyr
 *@date: 2025-10-11 16:56:32
 *@version: V1.0.0
****************/
#include "stm32f10x.h"
#include "./Driver/inc/MyDelay.h"
#include "./Driver/inc/OLED.h"
#include "./Driver/inc/LED.h"
#include "./Driver/inc/KEY.h"

int main()
{
    LED_Init();
    OLED_Init();
    OLED_Clear();
    KEY_Init();
/* 
    // 静态文字显示
    OLED_ShowString(1,1,"Env Detector ");
    OLED_ShowString(2,1,"Temperature: ");
    OLED_ShowString(3,1,"Light: ");
    OLED_ShowString(4,1,"Humidity: ");  */
    while (1)
    {
        if(KEY_Scan()){
            LED_Toggle();
        }
    }
}
