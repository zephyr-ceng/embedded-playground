#include "./drive/MyRename.h"
#include "./drive/Mykey.h"
#include "./drive/MyInterrupt.h"


uint8_t KeyNum,speed;

void main(void)
{
    Timer0_Init();
    while (1) {
        KeyNum = Check_KeyNum();
        if (KeyNum)
        {
            speed++;
            speed %=4; // 等于4时就置0
            // 按键控制电机转速
            if (speed == 0) duty = 10;
            if (speed == 1) duty = 50;
            if (speed == 2) duty = 75;
            if (speed == 3) duty = 100;
        }
        
    }
}