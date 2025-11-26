/****************
 *@description: C程序通用文件定义
 *@brief: 包含GPIO端口定义
 *@author: zephyr
 *@date: 2025-11-24 16:14:23
 *@version: V1.0.0
 ****************/

#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f10x.h"

#define FN_Pin       GPIO_Pin_0 // 开关机控制按键
#define FN_GPIO_Port GPIOB
#define CE_Pin       GPIO_Pin_3 // 74hc165 的CE端
#define CE_GPIO_Port GPIOB
#define PL_Pin       GPIO_Pin_4 // 74HC165的PL端
#define PL_GPIO_Port GPIOB

#endif // !__MAIN_H__
