/****************
 *@description: LCD1602 静态字符显示驱动程序
 *@author: zephyr
 *@date: 2025-02-03 10:53:03
 *@version: V1.0.0
 ****************/

/* 系统头文件 */
#include "./drive/89C52.H"
#include "./drive/intrins.h"
#include "./drive/MyDelay.h"
#include "./drive/MyRename.h"

/* 内部变量 */
#define DataP0 P0
sbit RW = P2 ^ 5;
sbit RS = P2 ^ 6;
sbit EN = P2 ^ 7;

/* 函数 */

/**
 *@description: LCD1602判忙函数
 *@parameters: null
 *@returnValue: bit 用于状态判定，1表示1602忙，设置EN高电平后判定DB7(BF)
 **/
bit LCD_Check_Busy(void)
{
    DataP0 = 0xff;

    RS = 0;
    RW = 1;
    EN = 0;
    _nop_();
    EN = 1;
    return (DataP0 & 0x80); // 保留DataP0最高位用于判定LCD状态
}
/**
 *@description: LCD1602写指令函数
 *@parameters: 8位二进制命令
 *@returnValue: null
 **/
void LCD_Write_Cmd(uint8_t command)
{
    while (LCD_Check_Busy());
    RS     = 0;
    RW     = 0;
    EN     = 1;
    DataP0 = command;
    _nop_();
    EN = 0;
}

/**
 *@description: LCD1602写数据函数
 *@parameters: data数据
 *@returnValue: null
 **/

void LCD_Write_Data(uint8_t Data)
{
    while (LCD_Check_Busy());
    RS     = 1;
    RW     = 0;
    EN     = 1;
    DataP0 = Data;
    _nop_();
    EN = 0;
}

/**
 *@description: LCD1602清屏函数
 *@parameters: null
 *@returnValue: null
 **/
void LCD_Clear(void)
{
    LCD_Write_Cmd(0x01);
    DelayMs(10);
}

/**
 *@description: LCD1602字符串写入函数
 *@parameters: n 字符列数 m 字符行数 s 指针变量 存储地址值
 *@returnValue: null
 **/
void LCD_Write_String(uint8_t n, uint8_t m, uint8_t *s)
{
    // 0x80表示第一行，0xC0：第二行
    m==0?LCD_Write_Cmd(0x80 + n):LCD_Write_Cmd(0xC0 + n);
    while (*s) {
        LCD_Write_Data(*s); // 将指针s的地址值全部写入LCD1602
        s++;
    }
}

/**
 *@description: LCD1602字符写入函数
 *@parameters: j 字符显示列数， k 字符显示行数，Data 字符代码
 *@returnValue: null
 **/
void LCD_Write_Char(uint8_t j, uint8_t k, uint8_t Data)
{
    // 0x80表示第一行，0xC0：第二行
    k==0?LCD_Write_Cmd(0x80 + j):LCD_Write_Cmd(0xC0 + j);
    LCD_Write_Data(Data);
}

/**
 *@description: LCD1602初始化
 *@parameters: null
 *@returnValue: null
 **/
void LCD_Init(void)
{
    LCD_Write_Cmd(0x38); // 将1602功能模式设为8位总线(DB4=1)、两行显示(DB3=1)、5×7点阵/每字符(DB2=0)
    DelayMs(5);
    LCD_Write_Cmd(0x08); // 将1602设为显示屏 关显示(DB2=0)、不显示光标(DB1=0) 、光标闪烁(DB0=0)
    LCD_Write_Cmd(0x01); // 清屏
    LCD_Write_Cmd(0x06); // 将1602设为输入字符时光标右移(DB1=1)、输入字符时显示屏的全部显示不移动(DB0=0) 
    DelayMs(5);
    LCD_Write_Cmd(0x0C); // 将1602设为显示屏 开显示(DB2=1)、不显示光标(DB1=0)、光标闪烁(DB0=0) 
}

// void main()
// {
//     /* 模块初始化 */
//     LCD_Init();
//     LCD_Clear();
//     /* 主循环 */
//     while (1) {
//         LCD_Write_Char(7, 0, 'o');
//         LCD_Write_Char(8, 0, 0x6B);
//         LCD_Write_String(1, 1, "www.etv100.com");
//         while(1);
//     }
// }