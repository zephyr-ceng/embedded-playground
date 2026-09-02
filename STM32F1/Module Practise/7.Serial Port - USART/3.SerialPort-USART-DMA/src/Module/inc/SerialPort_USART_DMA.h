/****************
 *@description: Serial Port USART DMA Header File
 *@author: zephyr
 *@date: 2025-09-10 18:03:51
 *@version: V1.0.0
****************/

#ifndef __SERIALPORT_USART_DMA_H
#define __SERIALPORT_USART_DMA_H

#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"

#define USART_RX_BUFFER_SIZE 255
#define USART_TX_BUFFER_SIZE 255

extern uint8_t RX_BUFFER[USART_RX_BUFFER_SIZE];
extern uint8_t TX_BUFFER[USART_TX_BUFFER_SIZE];

void USART_Init_GPIOConfig(void);
void USART_Init_Config(uint32_t baudrate);
uint16_t Get_ReviceData(uint8_t *Buf);
void USART_TransmitData(uint8_t *data,uint16_t length);
#endif // __SERIALPORT_USART_DMA_H

