/****************
 *@description: USART1 串口-中断收发
 *@author: zephyr
 *@date: 2025-06-05 11:34:16
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h" // Include DMA header


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
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);
}


/**
* @brief  初始化USART1的DMA通道
* @note   配置DMA通道1（RX）和通道4（TX）用于USART1的DMA传输，并处理接收缓冲区溢出。
* @param  null
* @retval null
*/
void DMA_Init_USART1(void) {
    // Enable DMA1 clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Configure DMA1 Channel4 (USART1_TX)
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; // USART1 data register
    DMA_InitStructure.DMA_MemoryBaseAddr = 0; // Will be set dynamically
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // Memory to peripheral
    DMA_InitStructure.DMA_BufferSize = 0; // Will be set dynamically
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    // Configure DMA1 Channel5 (USART1_RX)
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; // USART1 data register
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxBuffer; // Receive buffer
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // Peripheral to memory
    DMA_InitStructure.DMA_BufferSize = RX_BUFFER_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    
    // Enable USART1 DMA RX and TX
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

    // Enable DMA1 Channel5 (USART1_RX)
    DMA_Cmd(DMA1_Channel5, ENABLE);
}

/**
* @brief  检查接收缓冲区溢出并处理
* @note   如果缓冲区溢出，清除标志并丢弃旧数据。
* @param  null
* @retval null
*/
void CheckBufferOverflow(void) {
    if (bufferOverflow) {
        bufferOverflow = 0; // 清除溢出标志
        rxWriteIndex = 0;   // 重置写入索引
        rxReadIndex = 0;    // 重置读取索引
        printf("Buffer overflow occurred. Data has been reset.\n");
    }
}

/**
* @brief  使用DMA发送数据
* @note   此函数通过DMA通道发送指定的数据，并等待传输完成。
* @param  data: 指向要发送的数据的指针
* @param  length: 要发送的数据长度
* @retval null
*/
void DMA_SendData_USART1(const char *data, uint16_t length) {
    // TODO: undefined reference to `DMA_GetFlagStatus'
    if (length == 0 || data == NULL) {
        return; // 无效参数，直接返回
    }

    // Wait for any ongoing DMA transfer to complete
    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);

    // Disable DMA1 Channel4 (USART1_TX) before configuring
    DMA_Cmd(DMA1_Channel4, DISABLE);

    // Configure DMA1 Channel4 with new data and length
    DMA1_Channel4->CMAR = (uint32_t)data; // 设置内存地址
    DMA_SetCurrDataCounter(DMA1_Channel4, length); // 设置传输长度

    // Clear the transfer complete flag
    DMA_ClearFlag(DMA1_FLAG_TC4);

    // Enable DMA1 Channel4 (USART1_TX)
    DMA_Cmd(DMA1_Channel4, ENABLE);

    // Wait for the transfer to complete
    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);

    // Disable DMA1 Channel4 after transfer is complete
    DMA_Cmd(DMA1_Channel4, DISABLE);
}

/**
* @brief  从接收缓冲区读取所有数据
* @note   此函数从接收缓冲区中读取所有可用数据。
* @param  buffer: 指向存储读取数据的缓冲区
* @param  maxLength: 缓冲区的最大长度
* @retval 实际读取的数据长度
*/
int ReadAllFromBuffer(char *buffer, int maxLength) {
    int bytesRead = 0;

    while (rxReadIndex != rxWriteIndex && bytesRead < maxLength) {
        buffer[bytesRead++] = rxBuffer[rxReadIndex];
        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE; // 更新读取索引
    }

    return bytesRead;
}
