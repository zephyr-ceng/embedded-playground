/****************
 *@description: Serial Port USART DMA Header File
 *@author: zephyr
 *@date: 2025-09-10 18:03:51
 *@version: V1.0.0
****************/

#ifndef __SERIALPORT_USART_DMA_H
#define __SERIALPORT_USART_DMA_H

#include "stm32f10x.h"
// #include <stdbool.h>
#include "string.h"

void USART_InitModule(uint32_t baudrate);
void USART_ReceiveData_DMA(uint8_t *data,uint8_t len);
void USART_SendData_DMA(uint8_t *data, uint8_t length);
#endif // __SERIALPORT_USART_DMA_H