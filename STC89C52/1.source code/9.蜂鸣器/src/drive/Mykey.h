/****************
 *@description: 独立按键
 *@author: zephyr
 *@date: 2025-02-17 16:22:57
 *@version: V1.0.0
 ****************/

 #ifndef __Mykey_H__
 #define __Mykey_H__
 #include "89C52.h"
 #include "MyRename.h"
 #include "MyDelay.h"
 
 // 独立按键测试
 sbit K1 = P3 ^ 0;
 sbit K2 = P3 ^ 1;
 sbit K3 = P3 ^ 2;
 sbit K4 = P3 ^ 3;
 
 /**
  * @brief  独立按键返回
  * @param  Null
  * @retval keyNum 按键值 1~4
  * */
 uint8_t Check_KeyNum()
 {
     uint8_t keyNum = 0;
     if (K1 == 0) {
         DelayMs(20);
         while (K1 == 0); // 等待按键弹起
         keyNum = 1;
     }
     if (K2 == 0) {
         DelayMs(20);
         while (K2 == 0);
         keyNum = 2;
     }
     if (K3 == 0) {
         DelayMs(20);
         while (K3 == 0);
         keyNum = 3;
     }
     if (K4 == 0) {
         DelayMs(20);
         while (K4 == 0);
         keyNum = 4;
     }
     return keyNum;
 }
 
 #endif // !__Mykey_H__
 