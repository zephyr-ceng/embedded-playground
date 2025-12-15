#include "./Driver/inc/KEY.h"
#include "./Driver/inc/MyDelay.h"
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_initStructure;
    RCC_APB2PeriphClockCmd(KEY_PERIPH, ENABLE);
    GPIO_initStructure.GPIO_Pin = KEY_PIN;
    GPIO_initStructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
    GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIO_initStructure);
}

uint8_t KEY_Scan(void)
{
    if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ON)
    {
        Delay_ms(10); // 消抖
        if (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ON)
        {
            while (GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN) == KEY_ON); // 等待按键释放
            Delay_ms(10); // 消抖
            return 1;
        }
    }
    return 0;
}