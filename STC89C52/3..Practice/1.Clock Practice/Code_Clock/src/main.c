// #include "./drive/89C52.h"
#include "./drive/Intins.h"
#include "./drive/MyRename.h"
#include "./drive/MyDelay.h"
#include "./drive/MyLCD1602.h"
#include "./drive/MyKey.h"
#include "./drive/MyDS1302.h"

uint8_t keyNum, Time_Select,mode,flag;

/**
* @brief  Time0 中断0 的初始化，定时时间1000ms
* @param  Null
* @retval Null
* */
void Timer0_Init()
{
    IE   = 0x82;                // 设置ET0为1，使能定时器0中断，EA为1，启用全局中断
    TMOD = 0x01;                // 定时器0为模式1（16位定时器）0x02 13位定时器 0x03 8位自动重装定时器
    TCON |= 0x10;               // 设置TR0为1，启动定时器0 清除TF0: TCON &= ~0x10;
    TH0 = (65535 - 1000) / 256; // 定时2000ms
    TL0 = (65535 - 1000) % 256;
}

/**
* @brief  中断处理程序，将闪烁位取反
* @param  Null
* @retval Null
* */
void Timer0_ISR(void) interrupt 1
{
    flag = !flag;
    TH0  = (65535 - 1000) / 256; // 定时2000ms
    TL0  = (65535 - 1000) % 256;
}

/**
* @brief  获取时间，并将时间在LCD1602进行显示
* @param  Null
* @retval Null
* */
void Time_Show(void)
{
    DS1302_GetDateTime();
    LCD_ShowNum(1, 3, DS1302_DateTime[0], 2);
    LCD_ShowNum(1, 6, DS1302_DateTime[1], 2);
    LCD_ShowNum(1, 9, DS1302_DateTime[2], 2);
    LCD_ShowNum(2, 1, DS1302_DateTime[3], 2);
    LCD_ShowNum(2, 4, DS1302_DateTime[4], 2);
    LCD_ShowNum(2, 7, DS1302_DateTime[5], 2);
}

/**
* @brief  日期时间设置
* @param  Null
* @retval Null
* */

void Time_set(void)
{
    // 移位控制
    if (keyNum == 2) {
        Time_Select++;
        Time_Select %= 6;
    }
    // 加
    if (keyNum == 3) {
        // 年份
        DS1302_DateTime[Time_Select]++;
        if (DS1302_DateTime[0] > 99)
            DS1302_DateTime[0] = 0;

        // 月
        if (DS1302_DateTime[1] > 12)
            DS1302_DateTime[1] = 1;

        // 天数, 每月天数不一时需判断月份，1,3,5,7,8,10,12，为31天，4,6,9,11为30天，二月需判断闰年（%4==0）29天其余28天
        if (DS1302_DateTime[1] == 1 || DS1302_DateTime[1] == 3 || DS1302_DateTime[1] == 5 || DS1302_DateTime[1] == 7 ||
            DS1302_DateTime[1] == 8 || DS1302_DateTime[1] == 10 || DS1302_DateTime[1] == 12) {
            if (DS1302_DateTime[2] > 31) {
                DS1302_DateTime[2] = 1;
            }
        } else if (DS1302_DateTime[1] == 4 || DS1302_DateTime[1] == 6 || DS1302_DateTime[1] == 9 || DS1302_DateTime[1] == 11) {
            if (DS1302_DateTime[2] > 30) {
                DS1302_DateTime[2] = 1;
            }
        } else if (DS1302_DateTime[1] == 2) {
            if (DS1302_DateTime[0] % 4 == 0) {
                if (DS1302_DateTime[2] > 29) {
                    DS1302_DateTime[2] = 1;
                }
            } else {
                if (DS1302_DateTime[2] > 28) {
                    DS1302_DateTime[2] = 1;
                }
            }
        }
        // 时
        if (DS1302_DateTime[3] > 23)
            DS1302_DateTime[3] = 0;
        // 分
        if (DS1302_DateTime[4] > 59)
            DS1302_DateTime[4] = 0;
        // 秒
        if (DS1302_DateTime[5] > 59)
            DS1302_DateTime[5] = 0;
    }
    // 减
    if (keyNum == 4) {
        // 年
        DS1302_DateTime[Time_Select]--;
        if (DS1302_DateTime[0] < 0)
            DS1302_DateTime[0] = 99;
        // 月
        if (DS1302_DateTime[1] < 1)
            DS1302_DateTime[1] = 12;
        // 天 每月天数不一时需判断月份，1,3,5,7,8,10,12，为31天，4,6,9,11为30天，二月需判断闰年（%4==0）29天其余28天
        // 此外月份变更时，需要判断天数是否超过当月总天数，做对应更改，如五月变为四月，天数为31天时需要重新置1
        if (DS1302_DateTime[1] == 1 || DS1302_DateTime[1] == 3 || DS1302_DateTime[1] == 5 || DS1302_DateTime[1] == 7 ||
            DS1302_DateTime[1] == 8 || DS1302_DateTime[1] == 10 || DS1302_DateTime[1] == 12) {
            if (DS1302_DateTime[2] < 1)
                DS1302_DateTime[2] = 31;
            if (DS1302_DateTime[2] > 31)
                DS1302_DateTime[2] = 1;
        } else if (DS1302_DateTime[1] == 4 || DS1302_DateTime[1] == 6 || DS1302_DateTime[1] == 9 || DS1302_DateTime[1] == 11) {
            if (DS1302_DateTime[2] < 1)
                DS1302_DateTime[2] = 30;
            if (DS1302_DateTime[2] > 30)
                DS1302_DateTime[2] = 1;
        } else if (DS1302_DateTime[1] == 2) {
            if (DS1302_DateTime[0] % 4 == 0) {
                if (DS1302_DateTime[2] < 1)
                    DS1302_DateTime[2] = 29;
                if (DS1302_DateTime[2] > 29)
                    DS1302_DateTime[2] = 1;
            } else {
                if (DS1302_DateTime[2] < 1)
                    DS1302_DateTime[2] = 28;
                if (DS1302_DateTime[2] > 28)
                    DS1302_DateTime[2] = 1;
            }
        }
        // 时
        if (DS1302_DateTime[3] < 0)
            DS1302_DateTime[3] = 23;
        // 分
        if (DS1302_DateTime[4] < 0)
            DS1302_DateTime[4] = 59;
        // 秒
        if (DS1302_DateTime[5] < 0)
            DS1302_DateTime[5] = 59;
    }
    // 刷新数据,并保持选择位闪烁，选中显示空格，未选中显示原来的信息
    if (Time_Select == 0 && flag == 1)
        LCD_ShowString(1, 3, "  ");
    else
        LCD_ShowNum(1, 3, DS1302_DateTime[0], 2);
    if (Time_Select == 1 && flag == 1)
        LCD_ShowString(1, 6, "  ");
    else
        LCD_ShowNum(1, 6, DS1302_DateTime[1], 2);
    if (Time_Select == 2 && flag == 1)
        LCD_ShowString(1, 9, "  ");
    else
        LCD_ShowNum(1, 9, DS1302_DateTime[2], 2);
    if (Time_Select == 3 && flag == 1)
        LCD_ShowString(2, 1, "  ");
    else
        LCD_ShowNum(2, 1, DS1302_DateTime[3], 2);
    if (Time_Select == 4 && flag == 1)
        LCD_ShowString(2, 4, "  ");
    else
        LCD_ShowNum(2, 4, DS1302_DateTime[4], 2);
    if (Time_Select == 5 && flag == 1)
        LCD_ShowString(2, 7, "  ");
    else
        LCD_ShowNum(2, 7, DS1302_DateTime[5], 2);
}

void main(void)
{
    LCD_Init();
    DS1302_Init();
    Timer0_Init();
    DS1302_SetDateTime();
    LCD_ShowString(1, 1, "20  -  -  ");
    LCD_ShowString(2, 1, "  :  :  ");
    while (1) {
        keyNum = Check_KeyNum();
        // mode 1 为设置模式，0为显示模式
        if (keyNum == 1)
            if (mode == 0)
                mode = 1;
            else if (mode == 1) {
                Time_Select = 0;
                mode        = 0;
            }
        switch (mode) {
            case 0:
                Time_Show();
                break;
            case 1:

                Time_set();
                DS1302_SetDateTime();
                break;
        }
    }
}