/****************
 *@description: 旋转编码器-OLED数字加减
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/EC11.h"

int main(void)
{
    int16_t number=0;
    OLED_Init();
    EC11_Init();
    OLED_ShowString(1,1,"Count:");
    while (1) {
        number +=EC_Count_Get(); // 每触发一次加 1
        OLED_ShowSignedNum(2,1,number,6);
    }
}
