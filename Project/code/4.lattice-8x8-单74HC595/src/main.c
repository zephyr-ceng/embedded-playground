/****************
 *@description: LED点阵 + 74HC595(ST_CP锁存端 SH_CP移位端均上升沿有效)
 *@author: Ethan
 *@date: 2025-01-18 09:50:14
 *@version: V1.0.0
 ****************/

#include "./drive/89C52.H"
#include "./drive/my52.h"
#include "./drive/rename.h"
#include "./drive/intrins.h"
#define Matrix_Column P0

uint8_t code Column_table[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; // 行码
uint8_t code Row_table[]    = {0xff, 0xf7, 0xe7, 0xf7, 0xf7, 0xf7, 0xe3, 0xff}; // 数字 1 段码
uint8_t code animation[]    = {0x00, 0x00, 0x21, 0x21, 0x7F, 0x01, 0x00, 0x00};

// 行配置
// sbit Row_DS_MaZhi = P2 ^ 0; // 串行数据
// sbit Row_ST_Suo   = P2 ^ 1; // 锁存控制
// sbit Row_SH_YiWei = P2 ^ 2; // 移位控制

sbit SER = P2 ^ 0; // 串行数据
sbit RCK = P2 ^ 1; // 锁存
sbit SCK = P2 ^ 2; // 移位

void SendByte_74HC595(uint8_t dat)
{
    /**
     *@description: 74hc595参数发送
     *@parameters: dat: 发送的串行数据（先低后高）
     **/
    uint8_t i;
    for (i = 0; i < 8; i++) {
        SER = dat & (0x80 >> i);
        SCK = 1;
        SCK = 0;
    }
    RCK = 1;
    RCK = 0;
}

void MatrixLED_ShowColumn(uint8_t Column, dat)
{
    /**
     *@description: 控制列显示，低电平有效(即低电平位显示)
     *@parameters: column: 对应列（0~7）；dat：串行数据，1亮0灭
     **/
    SendByte_74HC595(dat);
    Matrix_Column = ~(0x80 >> Column);
    delayMs(1);
    Matrix_Column = 0xff;
}

/*
void sendByte(uint8_t dat)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        Column_yiWei = 0;
        Column_lieMa = dat & 0x80;
        Column_yiWei = 1;
        dat <<= 1;
    }
    Column_lieSuo = 0;
    _nop_;
    Column_lieSuo = 1;
}

void sendTwoByte(uint8_t dat1, uint8_t dat2)
{
    sendByte(dat1);
    sendByte(dat2);
}

void sendHM(uint8_t dat)
{
    uint8_t i;
    for (i = 0; i < 8; i++) {
        Row_yiWei  = 0;
        Row_hangMa = dat & 0x80;
        Row_yiWei  = 1;
        dat <<= 1;
    }
    Row_hangSuo = 0;
    _nop_;
    Row_hangSuo = 1;
}

*/

void main(void)
{
    uint8_t i;
    RCK = 0;
    SCK = 0;
    while (1) {
        for (i = 0; i < 8; i++) {
            MatrixLED_ShowColumn(i, animation[i]);
        }
    }
}