/****************
 *@description: USART 头文件
 *@@brief: USART1的功能实现
 *@author: zephyr
 *@date: 2025-11-14 18:44:38
 *@version: V1.0.0
****************/

#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"

#define USART_RX_BUFFER_SIZE 255
#define USART_TX_BUFFER_SIZE 255

extern uint8_t RX_BUFFER[USART_RX_BUFFER_SIZE];
extern uint8_t TX_BUFFER[USART_TX_BUFFER_SIZE];


#endif // !__USART_H__
