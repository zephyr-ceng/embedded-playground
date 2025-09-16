/****************
 *@description: ADC采集光敏信号
 *@author: zephyr
 *@date: 2025-09-16 16:53:05
 *@version: V1.0.0
****************/

#include "./Module/inc/ADC.h"
#include "./Module/inc/OLED.h"
#include "./Module/inc/MyDelay.h"

int main(){
    OLED_Init();
    ADC_InitModule();
    while (1)
    {
        OLED_ShowNum(1,1,ADC_GetValue(),6);
        OLED_ShowNum(2,1,ADC_GetAverage(10),6);
        OLED_ShowString(3,1,"Voltage: ");
        OLED_ShowSignedNum(3,11,ADC_GetVoltage(),4);
    }
    
}
