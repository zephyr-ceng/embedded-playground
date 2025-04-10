/****************
 *@description: 按键控制LED亮灭和蜂鸣器
 *@author: zephyr
 *@date: 2025-03-26 22:31:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/MyDelay.h"
#include "./drive/led.h"
#include "./drive/key.h"


int main(void)
{
    uint8_t keyNum;
    LED_Init();
    Key1_Init();
    Key2_Init();
    while (1) {
        keyNum = Key_Scan();
        if (keyNum == 1)
        {
            Turn_LED(GPIOA,GPIO_Pin_1);
        }
        if (keyNum == 2)
        {
            Turn_LED(GPIOA,GPIO_Pin_2);
        }
    }
}