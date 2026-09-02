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

// USART 引脚及模式定义
#define USART1_TX_PIN GPIO_Pin_9
#define USART1_TX_MODE GPIO_Mode_AF_PP

#define USART1_RX_PIN GPIO_Pin_10
#define USART1_RX_MODE GPIO_Mode_IN_FLOATING



// USART DMA 缓冲区大小及声明
#define USART_RX_BUFFER_SIZE 255
#define USART_TX_BUFFER_SIZE 255
extern uint8_t RX_BUFFER[USART_RX_BUFFER_SIZE];
extern uint8_t TX_BUFFER[USART_TX_BUFFER_SIZE];

void Serial_USART_Config(uint32_t baudrate);
void Serial_USART_SendString(char *str);
uint16_t Serial_USART_ReceiveData(uint8_t *Buf);
#endif // !__USART_H__
