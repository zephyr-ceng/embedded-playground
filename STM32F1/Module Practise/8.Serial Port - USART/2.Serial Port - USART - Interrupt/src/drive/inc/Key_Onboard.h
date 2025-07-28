/****************
 *@description: 板载按键检测
 *@author: zephyr
 *@date: 2025-03-30 15:57:15
 *@version: V1.0.0
****************/


#ifndef __Key_Onboard_H__
#define __Key_Onboard_H__

// 按键按下电平
typedef enum {
    KEY_LOW_TRIGGER  = 0,
    KEY_HIGH_TRIGGER = 1,
} KEY_TriggerLevel;

// 按键状态
typedef enum {
    KEY_UP   = 0,
    KEY_DOWN = 1,
    KEY_INIT = 2,
} KEY_Status;


void KEY_Init();
uint8_t KEY_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, KEY_TriggerLevel key_PressStatus);

#endif // !__Key_Onboard_H__