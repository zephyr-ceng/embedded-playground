#ifndef _USART_DMA_H
#define _USART_DMA_H

#include "stm32f10x.h"

void USART_DMA_Init(USART_TypeDef *USARTx, uint32_t BaudRate);
void Get_ReviceData(uint16_t *Buffer);
void Set_SendData(USART_TypeDef *USARTx,uint16_t *Buffer, uint16_t Length);

#endif // !_USART_DMA_H