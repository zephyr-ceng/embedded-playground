#ifndef SeralPort_H
#define SeralPort_H

#include "stm32f10x.h"
#include "string.h"

void SerialPort_USART_Init(void);
uint8_t SerialPort_USART_ReceiveData(uint8_t *data,uint16_t length);
void SerialPort_USART_SendData(uint8_t *data,uint16_t length);

#endif // SeralPort_H