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

int16_t num;

int main(void)
{

    OLED_Init(); // 初始化OLED
    encoder_init();
    OLED_ShowString(1, 1, "num:"); // 第一行第三列开始显示字符串hello word！

    while (1) {
        num += encoder_get();
        OLED_ShowSignedNum(2, 2, num, 4);
        Delay_ms(50);
    }
}