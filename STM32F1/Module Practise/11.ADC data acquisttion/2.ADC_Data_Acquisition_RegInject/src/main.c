/****************
 *@description: ADC 多通道数据采集光敏、温度、湿度（注入通道）
 *@author: zephyr
 *@date: 2025-10-10 17:19:32
 *@version: V1.0.0
****************/


#include "./Module/inc/ADC.h"
#include "./Module/inc/OLED.h"
#include "./Module/inc/MyDelay.h"

int main(){
    OLED_Init();
    ADC_InitModule();
    OLED_ShowString(1,1,"ADC Value: ");
    while (1)
    {
        OLED_ShowString(2,1,"Illumination: ");
        OLED_ShowSignedNum(2,15,ADC_GetVoltage_Illumination(5),4); // 光敏电压值
        OLED_ShowString(3,1,"Temperature: ");
        OLED_ShowSignedNum(3,14,ADC_GetVoltage_Temperature(5),4); // 温度电压值
        OLED_ShowString(4,1,"Humidity: ");  
        OLED_ShowSignedNum(4,11,ADC_GetVoltage_Humidity(5),4); // 湿度电压值
        Delay_ms(500);
    }
    
}


// TODO: 待验证
