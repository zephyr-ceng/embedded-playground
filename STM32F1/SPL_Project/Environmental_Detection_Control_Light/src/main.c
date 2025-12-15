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
#include "./Driver/inc/ADC.h"

int main()
{
    float temp = 0;
    LED_Init();
    OLED_Init();
    OLED_Clear();
    KEY_Init();
    ADC_Config_Init();

    /*
        // 静态文字显示
        OLED_ShowString(1,1,"Env Detector ");
        OLED_ShowString(2,1,"Temperature: ");
        OLED_ShowString(3,1,"Light: ");
        OLED_ShowString(4,1,"Humidity: ");  */
    while (1) {
        // 动态数据显示
        temp = ADC_GetVoltage_Temperature(5);
        OLED_ShowSignedNum(2, 13, (int)(temp * 100), 4); // 温度显示，扩大100倍显示两位小数
        if (temp < 1.5f)
            LED_SetDuty(100);
        else if (temp < 2.5f)
            LED_SetDuty(50);
        else
            LED_SetDuty(0);

        if (KEY_Scan()) {
            LED_Green_Toggle();
            LED_Blue_Toggle();
        }
        Delay_ms(500);
    }
}
