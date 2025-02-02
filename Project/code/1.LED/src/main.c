/****************
 *@description: LED demo
 *@author: Ethan
 *@date: 2025-01-14 14:43:09
 *@version: V1.0.0
 ****************/

#include "./drive/89C52.H"
#include "./drive/my52.h"
#include "./drive/interrupt.h"
#include "./drive/rename.h"

#define LED P2;


void main(void)
{
    unsigned char i;
    P2 = 0xff;
    while (1) {
        delayMs(2000);
        P2 = P2 >> 1;
        delayMs(2000);
    }

}
