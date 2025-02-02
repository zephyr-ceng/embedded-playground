/****************
 *@description: LED点阵 + 74HC595(ST_CP锁存端 SH_CP移位端均上升沿有效),注意时序图，需要同时输出码值，每次输出间隔添加空输出以去除重影
 *@author: Ethan
 *@date: 2025-01-18 09:50:14
 *@version: V1.0.0
 ****************/

#include "./drive/89C52.H"
#include "./drive/my52.h"
#include "./drive/rename.h"
#include "./drive/intrins.h"

uint8_t code Column[]       = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}; // 列控制
uint8_t code Column_Value[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
                            //    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f}; // 数字 1 码值

// 显示行控制
sbit Row_SER = P1 ^ 1; // 串行数据
sbit Row_RCK = P1 ^ 2; // 锁存控制
sbit Row_SCK = P1 ^ 0; // 移位控制

// 显示码值配置
sbit Column_SER = P2 ^ 0; // 串行数据
sbit Column_RCK = P2 ^ 1; // 锁存
sbit column_SCK = P2 ^ 2; // 移位

/* 显示码值控制 */
void MatrixLED_SendByte_Column_Value(uint8_t dat)
{
    /**
     *@description: 74hc595显示列控制
     *@parameters: dat: 发送的串行数据（先低后高）
     **/
    uint8_t i;
    for (i = 0; i < 8; i++) {
        column_SCK = 0;
        Column_SER = dat & (0x80 >> i);
        column_SCK = 1;
    }
}
void MatrixLED_Out595()
{
    /**
     *@description: 74HC595显示列控制--锁存端（ST_CP）控制
     *@parameters: 无
     *@returnValue: 无
     **/
    Column_RCK = 0;
    _nop_();
    Column_RCK = 1;
}
void MatrixLED_SendTwoByte_Column_Value(uint8_t dat1, uint8_t dat2)
{
    /**
     *@description: 多色灯控制
     *@parameters: 一个dat控制一种颜色
     *@returnValue: 无
     **/
    MatrixLED_SendByte_Column_Value(dat1);
    MatrixLED_SendByte_Column_Value(dat2);
}
/* 显示列控制 */
void MatrixLED_SendByte_Column(uint8_t dat)
{
    /**
     *@description: 74hc595 显示码值发送
     *@parameters: dat: 发送的串行数据（先低后高）
     **/
    uint8_t i;
    for (i = 0; i < 8; i++) {
        Row_SCK = 0;
        Row_SER = ~dat & (0x80 >> i);
        Row_SCK = 1;
    }
    Row_RCK = 0;
    _nop_();
    Row_RCK = 1;
}
/* 总控制 */
void MatrixLED_Send(uint8_t column, uint8_t column_Value)
{
    /**
     *@description: 74HC595串行数据发送
     *@parameters: column 显示列控制 column_Value 亮灯位置控制
     *@returnValue: 无
     **/
    MatrixLED_SendByte_Column(column);
    MatrixLED_SendByte_Column_Value(column_Value);
    MatrixLED_Out595();
    delayMs(1);
    // 发送0x00可以去除重影
    MatrixLED_SendByte_Column(0x00);
    MatrixLED_SendByte_Column_Value(0x00);
    MatrixLED_Out595();
}
/* 初始化 */
void MatrixLED_Init()
{
    /**
     *@description: 74HC595初始化
     *@parameters: 无
     *@returnValue: 无
     **/
    Row_RCK    = 0;
    Row_SCK    = 0;
    column_SCK = 0;
    Column_RCK = 0;
}
void main(void)
{
    uint8_t i, count = 0, offset = 0;
    MatrixLED_Init();
    while (1) {
        for (i = 0; i < 8; i++) {
            MatrixLED_Send(Column[i], Column_Value[i + offset]);
            count++;
            if(count >200)
            {
                // delayMs(500);
                offset ++;
                count = 0;
                if(offset >8)
                    offset = 0;
            }
        }
    }
}