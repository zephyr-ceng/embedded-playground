#include "./drive/MyLCD1602.h"
#include "./drive/MyRename.h"
#include "./drive/Mykey.h"
#include "./drive/MyDelay.h"

sbit Buzzer = P2^5;

/**
* @brief  蜂鸣器控制响应函数
* @param  tm 持续时间 tm * ms
* @param  ms 响应频率,最低1ms 即500us翻转一次,针对源蜂鸣器
* @retval Null
* */
void Buzzer_Time(uint16_t tm, uint16_t ms){
    uint8_t i;
    for ( i = 0; i < tm; i++)
    {
        Buzzer = !Buzzer;
        DelayMs(ms);
    }
    
}

void main(void)
{
    uint8_t KeyNum;    
    LCD_Init();
    while (1){
        KeyNum = Check_KeyNum();
        if (KeyNum)
        {
            Buzzer_Time(1,500);
            LCD_ShowNum(1,1,KeyNum,2);
        }
        
    }
}