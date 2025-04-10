/****************
 *@description: LED鐐归樀 + 74HC595(ST_CP閿佸瓨绔� SH_CP绉讳綅绔潎涓婂崌娌挎湁鏁�),娉ㄦ剰鏃跺簭鍥撅紝闇€瑕佸悓鏃惰緭鍑虹爜鍊硷紝姣忔杈撳嚭闂撮殧0x00娣诲姞绌鸿緭鍑轰互鍘婚櫎閲嶅奖
 * 浣跨敤鏂规硶锛氬惊鐜亶鍘咰olumn & Column_Value
 *@author: Ethan
 *@date: 2025-01-18 09:50:14
 *@version: V1.0.0
 ****************/

#ifndef __MyMatrixLED_H__
#define __MyMatrixLED_H__

#include "89C52.H"
#include "Intrins.h"
#include "MyRename.h"

uint8_t code Column[]       = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}; // 鍒楁帶鍒�
uint8_t code Column_Value[] = {0x00, 0x00, 0x21, 0x7f, 0x01, 0x01, 0x00, 0x00}; // 鏁板瓧 1 鐮佸€�

// 琛岄厤缃�
sbit Row_SER = P1 ^ 1; // 涓茶鏁版嵁
sbit Row_RCK = P1 ^ 2; // 閿佸瓨鎺у埗
sbit Row_SCK = P1 ^ 0; // 绉讳綅鎺у埗

// 鍒楅厤缃�--P0
sbit Column_SER = P2 ^ 0; // 涓茶鏁版嵁
sbit Column_RCK = P2 ^ 1; // 閿佸瓨
sbit column_SCK = P2 ^ 2; // 绉讳綅

/* 鏄剧ず鐮佸€兼帶鍒� */
void MatrixLED_SendByte_Column_Value(uint8_t dat)
{
    /**
     *@description: 74hc595鏄剧ず鍒楁帶鍒�
     *@parameters: dat: 鍙戦€佺殑涓茶鏁版嵁锛堝厛浣庡悗楂橈級
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
     *@description: 74HC595鏄剧ず鍒楁帶鍒�--閿佸瓨绔紙ST_CP锛夋帶鍒�
     *@parameters: 鏃�
     *@returnValue: 鏃�
     **/
    Column_RCK = 0;
    _nop_();
    Column_RCK = 1;
}
void MatrixLED_SendTwoByte_Column_Value(uint8_t dat1 uint8_t dat2)
{
    /**
     *@description: 澶氳壊鐏帶鍒�
     *@parameters: 涓€涓猟at鎺у埗涓€绉嶉鑹�
     *@returnValue: 鏃�
     **/
    MatrixLED_SendByte_Column_Value(dat1);
    MatrixLED_SendByte_Column_Value(dat2);
}
/* 鏄剧ず鍒楁帶鍒� */
void MatrixLED_SendByte_Column(uint8_t dat)
{
    /**
     *@description: 74hc595 鏄剧ず鐮佸€煎彂閫�
     *@parameters: dat: 鍙戦€佺殑涓茶鏁版嵁锛堝厛浣庡悗楂橈級
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
/* 鎬绘帶鍒� */
void MatrixLED_Send(uint8_t column, uint8_t column_Value)
{
    /**
     *@description: 74HC595涓茶鏁版嵁鍙戦€�
     *@parameters: column 鏄剧ず鍒楁帶鍒� column_Value 浜伅浣嶇疆鎺у埗
     *@returnValue: 鏃�
     **/
    MatrixLED_SendByte_Column(column);
    MatrixLED_SendByte_Column_Value(column_Value);
    MatrixLED_Out595();
    delayMs(1);
    // 鍙戦€�0x00鍙互鍘婚櫎閲嶅奖
    MatrixLED_SendByte_Column(0x00);
    MatrixLED_SendByte_Column_Value(0x00);
    MatrixLED_Out595();
}
/* 鍒濆鍖� */
void sendByte_Init()
{
    /**
     *@description: 74HC595鍒濆鍖�
     *@parameters: 鏃�
     *@returnValue: 鏃�
     **/
    Row_RCK    = 0;
    Row_SCK    = 0;
    column_SCK = 0;
    Column_RCK = 0;
}

#endif // !__MyMatrixLED_H__
