/****************
 *@description: 按键数字检测（翻转扫描）
 *@author: Ethan
 *@date: 2025-01-14 14:43:09
 *@version: V1.0.0
 ****************/

#include "./drive/89C52.H"
#include "./drive/my52.h"
#include "./drive/rename.h"

uint8_t DMtable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; // 0~F的段码
uint8_t code WMtable[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};                                                 // 位码

#define WDM   P0
#define KEYP1 P1

unsigned char TDATA[8];
sbit DuanSuo = P2 ^ 3;
sbit WeiSuo  = P2 ^ 4;

/* 数码管显示函数 */
void Display(uint8_t ShiWei, uint8_t WeiShu)
{
    static unsigned char i;
    WDM    = WMtable[i + ShiWei];
    WeiSuo = 1;
    WeiSuo = 0;

    WDM     = TDATA[i];
    DuanSuo = 1;
    DuanSuo = 0;

    i++;
    if (i == WeiShu)
        i = 0;
}

/* 键盘检测函数 翻转扫描 */
uint8_t key_matrix_detection(void)
{
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

/* 行列式扫描 */
uint8_t key_matrix_ranks(void){
    uint8_t keyM;
    KEYP1 = 0xf7; 
    if(KEYP1!=0xf7){
        delayMs(20);
        if(KEYP1!=0xf7)
        {
            switch (KEYP1)
            {
            case 0x77:keyM = 0;break;
            case 0xb7:keyM = 4;break;
            case 0xd7:keyM = 8;break;
            case 0xe7:keyM = 12;break;
            }
        }
    }
    while (KEYP1 !=0xf7)
    {
        return keyM;
    }
    
    // 二
    KEYP1 = 0xfb; 
    if(KEYP1!=0xfb){
        delayMs(20);
        if(KEYP1!=0xfb)
        {
            switch (KEYP1)
            {
            case 0x7b:keyM = 1;break;
            case 0xbb:keyM = 5;break;
            case 0xdb:keyM = 9;break;
            case 0xeb:keyM = 13;break;
            }
        }
    }
    while (KEYP1 !=0xfb)
    {
        return keyM;
    }
    // 三
    KEYP1 = 0xfd; 
    if(KEYP1!=0xfd){
        delayMs(20);
        if(KEYP1!=0xfd)
        {
            switch (KEYP1)
            {
            case 0x7d:keyM = 2;break;
            case 0xbd:keyM = 6;break;
            case 0xdd:keyM = 10;break;
            case 0xed:keyM = 14;break;
            }
        }
    }
    while (KEYP1 !=0xfd)
    {
        return keyM;
    }
    // 四
    KEYP1 = 0xfe; 
    if(KEYP1!=0xfe){
        delayMs(20);
        if(KEYP1!=0xfe)
        {
            switch (KEYP1)
            {
            case 0x7e:keyM = 3;break;
            case 0xbe:keyM = 7;break;
            case 0xde:keyM = 11;break;
            case 0xee:keyM = 15;break;
            }
        }
    }
    while (KEYP1 !=0xfe)
    {
        return keyM;
    }
}

/* 中断函数 */
void T0Int_S(void) // 函数名为T0Int_S，输入和输出参数均为void（空）
{
    TMOD = 0x01;
    // 将 TH0 寄存器清 0
    TL0 = 0; // 将 TL0 寄存器清 0
    EA  = 1; // 让 IE 寄存器的 EA＝1，打开总中断
    ET0 = 1; // 让 IE 寄存器的 ET0＝1，允许T0的中断请求
    TR0 = 1; // 让 TCON 寄存器的 TR0＝1，启动T0在TH0、TL0初值基础上开始计数
}

void T0Int_Z(void)
{
    TH0 = (65536 - 200) / 256;
    TL0 = (65536 - 200) % 256;
}

void main(void)
{
    uint8_t num;
    T0Int_S();
    while (1) {
        num = key_matrix_ranks();
        TDATA[0] = DMtable[num];
        Display(0,2);
    }
}