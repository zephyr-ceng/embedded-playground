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

#define USART_RX_BUFFER_SIZE 1024
#define USART_TX_BUFFER_SIZE 1024

extern uint32_t RX_BUFFER[USART_RX_BUFFER_SIZE];
extern uint32_t TX_BUFFER[USART_TX_BUFFER_SIZE];

typedef void (*usart_rx_callback_t) (uint32_t *buf,uint16_t len);


void USART_RegisterRxCallback(usart_rx_callback_t cb);
void USART_InitConfiguration(uint32_t baudrate);
uint8_t USART_TransmitData(uint16_t *data, uint16_t len);
#endif // __SERIALPORT_USART_DMA_H
