/****************
 *@description: delay function
 *@author: Ethan
 *@date: 2025-01-12 21:32:08
 *@version: V1.0.0
 ****************/

#ifndef __MyDelay_H__
#define __MyDelay_H__
// Œ¢√Î—” ±∫Ø ˝
void DelayUs(unsigned char tm)
{
    while (tm--);
}

// ∫¡√Î—” ±∫Ø ˝
void DelayMs(unsigned char ms)
{
    while (ms--) 
    {
        DelayUs(248);
        DelayUs(248);
    }
}

#endif // !__MyDelay_H__
