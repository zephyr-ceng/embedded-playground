#ifndef __MyBuzzer_H__
#define __MyBuzzer_H__

#include "89C52.h"
#include "Intins.h"
#include "MyDelay.h"

sbit Buzzer = P2 ^ 5;

/**
 * @brief  蜂鸣器控制响应函数
 * @param  tm 持续时间 tm * ms
 * @param  ms 响应频率,最低1ms 即500us翻转一次,针对源蜂鸣器
 * @retval Null
 * */
void Buzzer_Time(uint16_t tm, uint16_t ms)
{
    uint8_t i;
    for (i = 0; i < tm; i++) {
        Buzzer = !Buzzer;
        DelayMs(ms);
    }
}

#endif // !__MyBuzzer_H__
