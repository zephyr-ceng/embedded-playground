#include "89C52.h"
#include "MyRename.h"

sbit Motor = P1 ^ 0;
uint8_t compare, duty; // compare 比较值 duty占空比


void Timer0_Init()
{
    IE   = 0x82;       // 设置ET0为1，使能定时器0中断，EA为1，启用全局中断
    TMOD = 0x02;       // 定时器0为模式1 0x01（16位定时器）0x02 13位定时器 0x03 8位自动重装定时器
    TCON |= 0x10;      // 设置TR0为1，启动定时器0 清除TF0: TCON &= ~0x10;
    TH0 = (255 - 200); // 定时200 us
    TL0 = (255 - 200);
}

void Timer0_ISR(void) interrupt 1
{

    TH0 = (255 - 200); // 定时200 us
    TL0 = (255 - 200);
    compare++;
    compare %= 100;
    if (duty < compare)
        Motor = 0;
    else
        Motor = 1;
}