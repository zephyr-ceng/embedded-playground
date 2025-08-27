#ifndef __GPIO_H__

#include "stm32f10x.h"

// GPIO配置结构体
typedef struct {
    GPIO_TypeDef* GPIOx;           // GPIO端口
    uint16_t GPIO_Pin;             // GPIO引脚
    uint8_t GPIO_Mode;             // 模式
    uint8_t GPIO_Speed;            // 速度
} GPIO_Config_t;

void GPIO_InitPin(GPIO_Config_t config);
void GPIO_SetPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, bool state);
bool GPIO_GetPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#endif // !__GPIO_H__


