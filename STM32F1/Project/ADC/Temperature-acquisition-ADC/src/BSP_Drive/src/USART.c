#include "../inc/USART.h"

// USART函数实现
void USART_InitModule(USART_Config_t config)
{
    USART_InitTypeDef USART_InitStructure;
    
    // 开启USART时钟
    if (config.USARTx == USART1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    else if (config.USARTx == USART2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    else if (config.USARTx == USART3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    USART_InitStructure.USART_BaudRate = config.USART_BaudRate;
    USART_InitStructure.USART_WordLength = config.USART_WordLength;
    USART_InitStructure.USART_StopBits = config.USART_StopBits;
    USART_InitStructure.USART_Parity = config.USART_Parity;
    USART_InitStructure.USART_Mode = config.USART_Mode;
    USART_InitStructure.USART_HardwareFlowControl = config.USART_HardwareFlowControl;
    
    USART_Init(config.USARTx, &USART_InitStructure);
    USART_Cmd(config.USARTx, ENABLE);
}

void USART_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    USART_SendData(USARTx, data);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

void USART_SendString(USART_TypeDef* USARTx, const char* str)
{
    while (*str) {
        USART_SendByte(USARTx, *str++);
    }
}

void USART_SendData(USART_TypeDef* USARTx, uint8_t* data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++) {
        USART_SendByte(USARTx, data[i]);
    }
}

uint8_t USART_ReceiveByte(USART_TypeDef* USARTx)
{
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USARTx);
}

bool USART_IsDataAvailable(USART_TypeDef* USARTx)
{
    return USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET;
}