/****************
 *@description: delay function
 *@author: Ethan
 *@date: 2025-01-12 21:32:08
 *@version: V1.0.0
****************/


// 微秒延时函数
void delayUs(unsigned char tm){
    while (tm--);
}

// 毫秒延时函数
void delayMs(unsigned char ms){
    while (ms--)
    {
        delayUs(248);
        delayUs(248);
    }
}