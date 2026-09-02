/****************
 *@description: GPIO之按键检测（上拉输入）控制LED(推挽输出)
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

    // 初始化板载 LED 和两个板载按键
    LED_Init();
    Key1_Init();
    Key2_Init();

    while (1) {
        // 获取按键扫描结果
        keyNum = Key_Scan();

        // SW1（PA0）控制 LED1（PA1）
        if (keyNum == 1)
        {
            Turn_LED(GPIOA,GPIO_Pin_1);
        }

        // SW2（PC13）控制 LED2（PA2）
        if (keyNum == 2)
        {
            Turn_LED(GPIOA,GPIO_Pin_2);
        }
    }
}