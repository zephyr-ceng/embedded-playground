/****************
 *@description: STM32F103 USART1 DMA驱动
 *@file: SerialPort.h
 *@author: zephyr
 *@date: 2025-06-17 16:12:08
 *@version: V1.0.0
****************/

#ifndef SeralPort_H
#define SeralPort_H

typedef enum {
    TRANSMISSION_EMPTY = 0,
    TRANSMISSION_TIMEOUT,
    TRANSMISSION_SUCCESS
} TransmissionStatus;

void USART_DMA_Init(uint32_t baudrate);
TransmissionStatus USART_DMA_Send(char *data, uint16_t length);
uint16_t USART_DMA_Read(char *buffer, uint16_t maxLength);

#endif // SeralPort_H