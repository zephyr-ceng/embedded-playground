/****************
 *@description: GPIO之板载按键检测（key1上拉输入 & key2浮空输入）控制有源蜂鸣器(推挽输出)
 *@author: zephyr
 *@date: 2025-03-26 22:31:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Key_Onboard.h"
#include "./drive/inc/Buzzer.h"

int main(void)
{
    Buzzer_Init();
    KEY_Init();
    while (1) {
        if (
            // 如果按键按下会返回KEY_DOWN，蜂鸣器响
            (KEY_Scan(GPIOA, GPIO_Pin_0, KEY_HIGH_TRIGGER) == KEY_DOWN) ||
            (KEY_Scan(GPIOC, GPIO_Pin_13, KEY_HIGH_TRIGGER) == KEY_DOWN)
        ) {
            Buzzer_Set();
        }
    }
}