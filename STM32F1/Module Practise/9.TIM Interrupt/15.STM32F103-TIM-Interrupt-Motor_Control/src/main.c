/****************
 *@description: 按键控制电机
 *@author: zephyr
 *@date: 2025-10-16 18:06:19
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./driver/inc/MyDelay.h"
#include "./driver/inc/Motor_Control.h"
#include "./driver/inc/Key.h"

#define button 0

int main()
{
    uint8_t speed             = 10;
    uint8_t temp              = 0;
    uint8_t Motor_Runing_flag = 0;
    Key_GPIO_Init();
    Motor_Init();
    while (1) {
        if (Key_Scan()) {
            Motor_Runing_flag = !Motor_Runing_flag;
            Motor_Direction_Reverse(1);
            Motor_Set_Speed(speed); // 设置电机速度为10%
        }
        while (Motor_Runing_flag) {
            switch (Key_Scan()) {
                case 0: // 停止
                    while (speed != 0) {
                        speed -= 10;
                        Motor_Set_Speed(speed);
                        Delay_ms(10);
                    }
                    Motor_Runing_flag = !Motor_Runing_flag;
                    break;
                case 2: // 加速
                    if (speed < 100) speed += 10;
                    Motor_Set_Speed(speed);
                    Delay_ms(10);
                    break;
                    case 3: // 减速
                    if (speed > 0) speed -= 10;
                    Motor_Set_Speed(speed);
                    Delay_ms(10);
                    break;
                    case 4: // 反向至原速度
                    temp = speed;
                    speed -= 10;
                    if (speed == 0) {
                        Motor_Direction_Reverse(0);
                        while (speed < temp) {
                            speed += 10;
                            Motor_Set_Speed(speed);
                            Delay_ms(10);
                        }
                    }
            }
        }
    }
}

