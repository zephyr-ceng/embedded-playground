/****************
 *@description: 矩阵按键扫描
 *@author: Ethan
 *@date: 2025-01-19 23:37:21
 *@version: V1.0.0
****************/

#ifndef __MyMatrixKEY_H__
#define __MyMatrixKEY_H__


#include "MyRename.h"
#define KEYP1 P0
// 翻转式扫描
uint8_t key_matrix_detection(void)
{
    /**
     *@description: 获取按键具体16进制码值
     *@parameters: 无
     *@returnValue: 16进制数据
    **/
    uint8_t keyM,row,column;
    KEYP1 = 0xf0;
    if(KEYP1 != 0xf0)
    {
        delayMs(20);
        KEYP1 = 0xf0;
        if(KEYP1 != 0xf0)
        {
            row = KEYP1;
            KEYP1 = 0x0f;
            column = KEYP1;
            keyM = row | column;
        }
    }else
        keyM = 0xff;
    return keyM;
}

uint8_t key_matrix_Value(void)
{
    /**
     *@description: 按键键值获取
     *@parameters: 无
     *@returnValue: 按键具体键值
    **/
    switch (key_matrix_detection()) {

        case 0xee:return 15;break; // c
        case 0xed:return 14;break; // d
        case 0xeb:return 13;break; // e
        case 0xe7:return 12;break; // f
        case 0xde:return 11;break; // 8
        case 0xdd:return 10;break; // 9
        case 0xdb:return 9;break; // a
        case 0xd7:return 8;break; // b
        case 0xbe:return 7;break; // 4    1011 1110
        case 0xbd:return 6;break; // 5
        case 0xbb:return 5;break; // 6
        case 0xb7:return 4;break; // 7
        case 0x7e:return 3;break; // 0 按下相应的键显示相对应的码值
        case 0x7d:return 2;break; // 1
        case 0x7b:return 1;break; // 2
        case 0x77:return 0;break; // 3
        default:return 0xff;break;
    }
}

#endif // !__MyMatrixKEY_H__
