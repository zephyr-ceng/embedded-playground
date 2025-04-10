/****************
 *@description: delay function
 *@author: zephyr
 *@date: 2025-01-12 21:32:08
 *@version: V1.0.0
 ****************/

 #ifndef __MyDelay_H__
 #define __MyDelay_H__
 // 微秒延时函数
 void DelayUs(unsigned char tm)
 {
     while (tm--);
 }
 
 // 毫秒延时函数
 void DelayMs(unsigned char ms)
 {
     while (ms--) {
         DelayUs(248);
         DelayUs(248);
     }
 }
 
 #endif // !__MyDelay_H__
 