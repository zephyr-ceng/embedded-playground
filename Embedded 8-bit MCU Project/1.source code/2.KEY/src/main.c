
#include <reg51.h>
#include "./drive/my52.h"

// sbit KeyAdd = P3 ^ 0;
// sbit KeyDec = P3 ^ 1;
#define WDM P0

sbit DuanSuo = P2 ^ 3;
sbit WeiSuo  = P2 ^ 4;

unsigned char code DMtable[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
unsigned char code WMtable[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
unsigned char TData[8];

void Display(unsigned char ShiWei, unsigned char WeiShu);

void main(void)
{
    unsigned char num = 10;
    while (1) {
        TData[1] = DMtable[num/10];
        TData[0] = DMtable[num%10];
        Display(0,2);
    }
}

void Display(unsigned char ShiWei, unsigned char WeiShu)
{
    unsigned char i;
    for (i = 0; i < WeiShu; i++) {
        WDM     = WMtable[i + ShiWei];
        WeiSuo  = 1;
        WeiSuo  = 0;
        WDM     = TData[i];
        DuanSuo = 1;
        DuanSuo = 0;
        delayMs(2);
    }
}