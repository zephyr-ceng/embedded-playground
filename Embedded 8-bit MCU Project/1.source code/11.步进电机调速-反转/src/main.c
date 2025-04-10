#include "./drive/Mykey.h"
#include "./drive/MyRename.h"
#include "./drive/motor.h"
#include "./drive/MyLCD1602.h"


uint8_t keyNum;
void main(void)
{
    LCD_Init();
    LCD_ShowNum(1,1,speed_delay,3);
    while (1) {
        keyNum = Check_KeyNum();
        if (keyNum) {
            // 正
            if (keyNum == 1) {
                forward();
                LCD_ShowNum(1,1,speed_delay,3);
            }
            // 反
            if (keyNum == 2) {
                backward();
                LCD_ShowNum(1,1,speed_delay,3);
            }
            // 加
            if (keyNum == 3) {
                accelerate();
                LCD_ShowNum(1,1,speed_delay,3);
            }
            // 减
            if (keyNum == 4) {
                decelerate();
                LCD_ShowNum(1,1,speed_delay,3);
            }
            // 停止
            if (keyNum == 5) {
                stop_motor();
            }
        }
    }
}