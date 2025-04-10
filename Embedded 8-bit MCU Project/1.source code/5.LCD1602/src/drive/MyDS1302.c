/****************
 *@description: DS1302时间日期设置，只需要调取SetDateTime & GetDateTime
 *@author: zephyr
 *@date: 2025-02-17 15:27:31
 *@version: V1.0.0
 ****************/

// 调用模块
#include "89C52.H"
#include "MyRename.h"
#include "Intrins.h"


sbit DS1302_CE   = P3 ^ 5; // CE 引脚
sbit DS1302_SCLK = P3 ^ 6; // SCLK 引脚
sbit DS1302_IO   = P3 ^ 4; // I/O 引脚

char DS1302_DateTime[] = {25,02,18,14,20,25};
void DS1302_WriteByte(unsigned char dat)
{
    unsigned char i;
    for (i = 0; i < 8; i++) {
        DS1302_IO = dat & 0x01; // 发送最低位
        dat >>= 1;              // 右移一位
        DS1302_SCLK = 1;        // 上升沿写入数据
        _nop_();
        DS1302_SCLK = 0; // 下降沿
        _nop_();
    }
}

// 从 DS1302 读取一个字节
unsigned char DS1302_ReadByte()
{
    unsigned char i, dat = 0;
    for (i = 0; i < 8; i++) {
        dat >>= 1;                  // 右移一位
        if (DS1302_IO) dat |= 0x80; // 读取数据位
        DS1302_SCLK = 1;            // 上升沿
        _nop_();
        DS1302_SCLK = 0; // 下降沿
        _nop_();
    }
    return dat;
}

/**
 * @brief  向DS1302写入数据
 * @param  addr 控制指令
 * @param  dat 写入的数据
 * @retval Null
 * */
void DS1302_Write(unsigned char addr, unsigned char dat)
{
    DS1302_CE = 1;          // 使能 DS1302
    DS1302_WriteByte(addr); // 发送地址
    DS1302_WriteByte(dat);  // 发送数据
    DS1302_CE = 0;          // 关闭 DS1302
}

/**
 * @brief  从 DS1302 读取数据
 * @param  addr 读取指令（和写入指令一样）
 * @retval dat 读取的数据
 * */
unsigned char DS1302_Read(unsigned char addr)
{
    unsigned char dat;
    DS1302_CE = 1;                 // 使能 DS1302
    DS1302_WriteByte(addr | 0x01); // 发送读命令,设置读取和设置命令一样
    dat       = DS1302_ReadByte(); // 读取数据
    DS1302_CE = 0;                 // 关闭 DS1302
    return dat;
}

/**
 * @brief  DS1302初始化
 * @param  Null
 * @retval Null
 * */
void DS1302_Init()
{
    DS1302_CE   = 0;          // 关闭 DS1302
    DS1302_SCLK = 0;          // 初始化 SCLK
    DS1302_Write(0x8E, 0x00); // 关闭写保护
    DS1302_Write(0x80, 0x00); // 启动时钟
}

/**
 * @brief  日期时间设置
 * @param  Null
 * @retval Null
 * */
void DS1302_SetDateTime()
{
    DS1302_Write(0x8E, 0x00);                                                   // 关闭写保护
    DS1302_Write(0x80, DS1302_DateTime[5] / 10 * 16 + DS1302_DateTime[5] % 10); // 设置秒
    DS1302_Write(0x82, DS1302_DateTime[4] / 10 * 16 + DS1302_DateTime[4] % 10); // 设置分
    DS1302_Write(0x84, DS1302_DateTime[3] / 10 * 16 + DS1302_DateTime[3] % 10); // 设置时
    DS1302_Write(0x86, DS1302_DateTime[2] / 10 * 16 + DS1302_DateTime[2] % 10); // 设置日
    DS1302_Write(0x88, DS1302_DateTime[1] / 10 * 16 + DS1302_DateTime[1] % 10); // 设置月
    DS1302_Write(0x8C, DS1302_DateTime[0] / 10 * 16 + DS1302_DateTime[0] % 10); // 设置年
    DS1302_Write(0x8E, 0x80);                                                   // 打开写保护
}

/**
 * @brief  日期时间读取
 * @param  Null
 * @retval Null
 * */
void DS1302_GetDateTime(void)
{
    uint8_t temp;
    temp               = DS1302_Read(0x80);
    DS1302_DateTime[5] = temp / 16 * 10 + temp % 16;
    temp               = DS1302_Read(0x82);
    DS1302_DateTime[4] = temp / 16 * 10 + temp % 16;
    temp               = DS1302_Read(0x84);
    DS1302_DateTime[3] = temp / 16 * 10 + temp % 16;
    temp               = DS1302_Read(0x86);
    DS1302_DateTime[2] = temp / 16 * 10 + temp % 16;
    temp               = DS1302_Read(0x88);
    DS1302_DateTime[1] = temp / 16 * 10 + temp % 16;
    temp               = DS1302_Read(0x8C);
    DS1302_DateTime[0] = temp / 16 * 10 + temp % 16;
}