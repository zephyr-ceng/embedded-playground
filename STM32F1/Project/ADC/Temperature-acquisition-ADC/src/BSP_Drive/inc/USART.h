#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"

// USART配置结构体
typedef struct {
    USART_TypeDef* USARTx;         // USART模块
    uint32_t USART_BaudRate;       // 波特率
} USART_Config_t;



void USART_InitModule(USART_Config_t config);

// TODO: 后续在Moduless 中具体实现
/* void USART_SendByte(USART_TypeDef* USARTx, uint8_t data);
void USART_SendString(USART_TypeDef* USARTx, const char* str);
void USART_SendData(USART_TypeDef* USARTx, uint8_t* data, uint16_t length);
uint8_t USART_ReceiveByte(USART_TypeDef* USARTx);
bool USART_IsDataAvailable(USART_TypeDef* USARTx); */


#endif // !__USART_H__
