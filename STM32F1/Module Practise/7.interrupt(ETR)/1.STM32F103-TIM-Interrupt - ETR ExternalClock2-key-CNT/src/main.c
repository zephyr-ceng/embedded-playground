/****************
 *@description: TIM1 使用 ETR 外部时钟计数，并显示到 OLED
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "./drive/inc/Timer.h"

int main(void)
{
    uint16_t Debounced_Count;
    uint8_t Key_Released = 1;

    OLED_Init();
    Timer_Init();
    // 从 TIM1 的实际计数值开始显示，例如 Timer_Init 中设置的 65530。
    // 使用 uint16_t，使软件显示计数也按照 0~65535 回绕。
    Debounced_Count = Get_Value();
    OLED_ShowString(1, 1, "ETR Count:");
    OLED_ShowString(3, 1, "PA12:");
    while (1) {
        // 按键接地，低电平表示按下；延时 20 ms 确认按键状态，过滤抖动
        if (Key_Released && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET) {
            Delay_ms(20);
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_RESET) {
                Debounced_Count++;
                Key_Released = 0;
            }
        }

        // 等待按键释放，并用 20 ms 延时确认释放状态
        if (!Key_Released && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_SET) {
            Delay_ms(20);
            if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == Bit_SET) {
                Key_Released = 1;
            }
        }

        OLED_ShowNum(2, 1, Debounced_Count, 6);
        OLED_ShowNum(3, 7, GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12), 1);
    }
}