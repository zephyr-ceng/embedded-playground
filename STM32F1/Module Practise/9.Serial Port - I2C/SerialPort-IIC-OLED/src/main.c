/****************
 *@description: Oled display example using IIC
 *@author: zephyr
 *@date: 2025-08-07 14:48:03
 *@version: V1.0.0
 ****************/

 #include "./Module/inc/MyDelay.h"
 #include "string.h"
 #include "./Module/inc/OLED_IIC.h"
 
 int main(void)
 {
    OLED_Init();
    OLED_ShowNum(1,1,32,4);
    OLED_ShowString(2,1,"Hello STM32");
    OLED_ShowBinNum(3,1,01110011,8);
    OLED_ShowHexNum(4,1,0xff,8);
    while (1)
    {
        // code
    }
    
 }
 
 
 // TODO: OLED待测试
 