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
    while (1)
    {
        OLED_ShowNum(1,1,32,4)
    }
    
 }
 
 
 // TODO: OLED待测试
 