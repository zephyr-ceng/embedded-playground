#include "./drive/89C52.h"
#include "./drive/Intins.h"
#include "./drive/MyDelay.h"
#include "./drive/MyRename.h"

sbit LED = P2 ^ 0;

/**
* @brief  PWM 占空比设置
* @param  duty 占空比 
* @retval Null
* */
void PWM_Control(uint8_t duty) {
    uint8_t i;
    // duty 逐渐变大 LED=0会逐渐变多 亮度就变亮
    for (i = 0; i < 255; i++) {
        if (i < duty) {
            LED = 0; // 低电平
        } else {
            LED = 1; // 高电平
        }
    }
}

/**
* @brief  LED 呼吸灯 周期为255
* @param  Null
* @retval Null
* */
void LED_flip()
{
    uint8_t i;
    // 逐渐变亮 
    for (i = 0; i < 255; i++) {
        PWM_Control(i); // 设置 PWM 占空比 i 变大 亮度就变亮
        DelayMs(5);       // 延时，控制渐变速度
    }

    // 逐渐变暗
    for (i = 255; i > 0; i--) {
        PWM_Control(i); // 设置 PWM 占空比 i 减小 亮度就减小
        DelayMs(5);       // 延时，控制渐变速度
    }
}

void main(void)
{
    while (1) {
        LED_flip();
    }
}

