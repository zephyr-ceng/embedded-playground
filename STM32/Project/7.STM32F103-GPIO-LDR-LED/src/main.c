/****************
 *@description: LDR控制无源蜂鸣器
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
// #include "./drive/inc/OLED.h"
#include "./drive/inc/LDR.h"
#include "./drive/inc/Buzzer.h"
#include "./drive/inc/Led_Onboard.h"

int main(void)
{
    // uint8_t value;
    LED_Init();
    // LDR_Init();
    Buzzer_Init();
    while (1) {
        if (Read_Light_Sensor()==1)
        {
            LED_Set_ON(GPIOA,GPIO_Pin_1);
            Delay_ms(500);
        }else{
            LED_Set_OFF(GPIOA,GPIO_Pin_1);
            Delay_ms(500);
        };
        Buzzer_Turn();
    }
}
