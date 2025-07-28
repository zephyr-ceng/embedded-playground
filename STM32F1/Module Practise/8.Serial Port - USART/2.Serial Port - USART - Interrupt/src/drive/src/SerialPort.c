/****************
 *@description: USART1 串口-中断收发
 *@author: zephyr
 *@date: 2025-06-05 11:34:16
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
// #include "stm32f10x_usart.h"
// #include "stm32f10x_gpio.h"
// #include <stdio.h>
// #include "stm32f10x_rcc.h"


#define RX_BUFFER_SIZE 128 // 接收缓冲区大小

volatile char rxBuffer[RX_BUFFER_SIZE]; // 接收缓冲区
volatile uint16_t rxWriteIndex = 0;     // 写入索引
volatile uint16_t rxReadIndex = 0;      // 读取索引
volatile uint8_t bufferOverflow = 0;   // 缓冲区溢出标志

/**
* @brief  初始化USART1
* @note   此函数配置USART1的波特率、数据位、停止位和校验位，并设置GPIO引脚。
* @param  null
* @retval null
* */
void UART_Init(void) {
    // Enable clocks for GPIOA and USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // Configure PA9 (TX) as alternate function push-pull
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure PA10 (RX) as input floating
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // Configure NVIC for USART1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable USART1 receive interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);
}


/**
* @brief  USART1中断处理程序
* @note   处理接收到的数据，包括单字节和字符串，并处理缓冲区溢出。
* @param  null
* @retval null
*/
void USART1_IRQHandler(void) {
    // 检查是否为接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // 读取接收到的数据
        char received = (char)USART_ReceiveData(USART1);

        // 检查缓冲区是否已满
        uint16_t nextIndex = (rxWriteIndex + 1) % RX_BUFFER_SIZE;
        if (nextIndex == rxReadIndex) {
            bufferOverflow = 1; // 设置缓冲区溢出标志
        } else {
            // 将数据存入缓冲区
            rxBuffer[rxWriteIndex] = received;
            rxWriteIndex = nextIndex;
        }

        // 清除中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/**
* @brief  从接收缓冲区读取单个字节
* @note   如果缓冲区为空，则返回0。
* @param  null
* @retval 接收到的字节
*/
uint8_t UART_ReadByteFromBuffer(void) {
    if (rxReadIndex == rxWriteIndex) {
        return 0; // 缓冲区为空
    } else {
        char data = rxBuffer[rxReadIndex];
        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
        return (uint8_t)data;
    }
}

/**
* @brief  检查缓冲区溢出状态
* @note   如果发生溢出，返回1，并清除溢出标志。
* @param  null
* @retval 缓冲区溢出状态
*/
uint8_t UART_CheckBufferOverflow(void) {
    if (bufferOverflow) {
        bufferOverflow = 0; // 清除溢出标志
        return 1;
    }
    return 0;
}

/**
* @brief  从接收缓冲区读取所有内容
* @note   将缓冲区中的所有数据读取到目标缓冲区中，直到缓冲区为空。
* @param  destBuffer: 存储读取数据的目标缓冲区
* @param  maxLength: 目标缓冲区的最大长度
* @retval 实际读取的字节数
*/
uint16_t UART_ReadAllFromBuffer(char *destBuffer, uint16_t maxLength) {
    uint16_t bytesRead = 0;
    while (rxReadIndex != rxWriteIndex && bytesRead < maxLength) {
        destBuffer[bytesRead++] = rxBuffer[rxReadIndex];
        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
    }
    return bytesRead;
}

/**
* @brief  发送字符串到USART1
* @param  str: 要发送的字符串
* @retval Null
* */

void UART_send_str_IT(const char *str) {
    while (*str != '\0') {
        // 等待发送缓冲区空
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        // 发送数据
        USART_SendData(USART1, (uint8_t)(*str++));
    }
    // 等待发送完成
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}
