#include "stm32f10x.h"
#include "./driver/inc/Ultrasonic_ranging.h"
#include "./driver/inc/OLED.h"
int main()
{
    OLED_Init();
    Ultrasonic_Ranging_Init();
    while (1)
    {
        OLED_ShowFloat(1,1,Ultrasonic_Ranging_GetDistance(),3,3); // 实时显示超声波的测距值
    }
}
