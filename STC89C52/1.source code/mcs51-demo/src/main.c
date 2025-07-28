#include "./drive/MyLCD1602.h"

void main(void)
{
    LCD_Init();
    LCD_ShowString(1, 1, "20  -  -  ");
    LCD_ShowString(2, 1, "  :  :  ");
    // while (1);
}