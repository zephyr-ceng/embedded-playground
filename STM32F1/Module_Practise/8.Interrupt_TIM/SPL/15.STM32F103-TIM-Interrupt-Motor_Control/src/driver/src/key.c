#include "./driver/inc/Key.h"

uint8_t KEY_RUNING = 0;

// GPIO初始化
void Key_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(KEY_CLK, ENABLE);                                                             // 使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin   = KEY_POWER | KEY_SPEED_UP_PIN | KEY_SPEED_DOWN_PIN | KEY_REVERSE_PIN; // PA0
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;                                                       // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_Status(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0) // 检测到按键按下
    {
        Delay_ms(10);                                    // 消抖
        if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0) // 再次检测按键状态
        {
            while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 0)
                return 1; // 返回按键按下标志
        }
    }
    return 0; // 按键未按下
}

uint8_t Key_Scan(void)
{
    if (Key_Status(KEY_PORT, KEY_POWER)) return !KEY_RUNING; // 开始和停止
    if (Key_Status(KEY_PORT, KEY_SPEED_UP_PIN)) return 2;    // 加速
    if (Key_Status(KEY_PORT, KEY_SPEED_DOWN_PIN)) return 3;  // 减速
    if (Key_Status(KEY_PORT, KEY_REVERSE_PIN)) return 4;     // 反向
    return 0;                                                // 无按键按下
}
