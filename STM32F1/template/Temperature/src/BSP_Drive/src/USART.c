#include "../inc/USART.h"

/**
* @brief  USART 初始化，含闲时中断设置
* @param  config: USART初始化参数的结构体
* @retval Null
* */
void USART_InitModule(USART_Config_t config)
{
    USART_InitTypeDef USART_InitStructure;
    
    // 开启USART时钟
    if (config.USARTx == USART1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    else if (config.USARTx == USART2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    else if (config.USARTx == USART3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    USART_InitStructure.USART_BaudRate = config.USART_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不使用硬件流控制
    
    USART_Init(config.USARTx, &USART_InitStructure);
    USART_ITConfig(config.USARTx, USART_IT_IDLE, ENABLE); // 使能空闲中断
    USART_Cmd(config.USARTx, ENABLE);
}


// TODO: 后续在Moduless 中具体实现
/* 
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
} */