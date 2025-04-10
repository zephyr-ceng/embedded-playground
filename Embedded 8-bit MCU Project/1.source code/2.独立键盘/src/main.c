#include "./drive/Mykey.h"
#include "./drive/rename.h"
#include "./drive/MyLCD1602.h"


void main(void)
{
    uint8_t keyNum;
    LCD_Init();
    while (1)
    {
        keyNum = Check_KeyNum();
        if(keyNum)
            LCD_ShowNum(1,1,keyNum,2);  
    }
}
