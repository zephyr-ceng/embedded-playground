/****************
 *@description: LED状态翻转
 *@author: zephyr
 *@date: 2025-03-30 16:01:18
 *@version: V1.0.0
****************/

#ifndef __Led_Onboard_H__
#define __Led_Onboard_H__

// 灯的状态
typedef enum{
    LED_ON = 1,
    LED_OFF = 0,
}LED_Status;

void LED_Init();
void LED_Set_ON(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void LED_Set_OFF(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void Turn_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif // !__Led_Onboard_H__
