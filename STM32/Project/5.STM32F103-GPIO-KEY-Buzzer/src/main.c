/****************
 *@description: 板载按键浮空输入控制LED亮灭
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
            (KEY_Scan(GPIOA, GPIO_Pin_0, KEY_HIGH_TRIGGER) == KEY_DOWN) ||
            (KEY_Scan(GPIOC, GPIO_Pin_13, KEY_HIGH_TRIGGER) == KEY_DOWN)
        ) {
            Buzzer_Set();
        }
    }
}