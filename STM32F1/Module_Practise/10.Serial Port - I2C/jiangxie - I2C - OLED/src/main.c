/****************
 *@description: 板载按键浮空输入控制LED亮灭
 *@author: zephyr
 *@date: 2025-03-26 22:31:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/OLED.h"


int main(void)
{
    OLED_Init();
    OLED_ShowNum(1,1,12,4);
    OLED_ShowNum(2,1,12,4);
    OLED_ShowString(3,1,"hello world");
    while (1) {
    }
}