/****************
 *@description: 舵机控制
 *@author: zephyr
 *@date: 2025-10-17 17:58:43
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "./driver/inc/Servo_Control.h"
#include "./driver/inc/MyDelay.h"

int main()
{
    SERVO_Init();
    while (1)
    {
        Servo_SetAngle(30);
        Delay_ms(20);
        Servo_SetAngle(90);
        Delay_ms(20);
    }
}
