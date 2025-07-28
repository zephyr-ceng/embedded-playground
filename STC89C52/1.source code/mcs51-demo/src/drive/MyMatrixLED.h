/****************
 *@description: 74H573 8位数码管驱动
 *@author: zephyr
 *@date: 2025-02-24 17:32:02
 *@version: V1.0.0
****************/

#include "89C52.h"
#include "MyDelay.h"

sbit LE1 = P2 ^ 3; // 段选锁存使能
sbit LE2 = P2 ^ 4; // 位选锁存使能

// 数码管段选编码表（0-9）
uint8_t code seg_code[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// 数码管位选编码表（选择哪一位数码管）
uint8_t code bit_code[] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

// 向锁存器写入数据
void Latch_Write(uint8_t dat, bit latch)
{
    P0 = dat; // 将数据写入 P0 口
    if (latch == 0) {
        LE1 = 1; // 锁存段选数据
        LE1 = 0;
    } else {
        LE2 = 1; // 锁存位选数据
        LE2 = 0;
    }
}

// 初始化数码管
void Dig_Init()
{
    Latch_Write(0xFF, 0); // 关闭所有段选
    Latch_Write(0x00, 1); // 关闭所有位选
}

// 在指定位置显示数字
void Dig_Display(uint8_t pos, uint8_t num)
{
    Latch_Write(seg_code[num], 0); // 写入段选数据
    Latch_Write(bit_code[pos], 1); // 写入位选数据
    DelayMs(5);                    // 延时，控制显示亮度
    Latch_Write(0x00, 1);          // 关闭位选，消除鬼影
}
