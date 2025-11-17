#ifndef __SerialPort_USART_H__
#define __SerialPort_USART_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#define USART_RX_BUFFER_SIZE 256
#define USART_TX_BUFFER_SIZE 256

extern uint8_t USART_RxBuffer[USART_RX_BUFFER_SIZE];
extern uint8_t USART_TxBuffer[USART_TX_BUFFER_SIZE];
extern uint16_t recv_length;
#define USART_BAUDRATE 115200
/* typedef struct{
    USART_TypeDef *USARTx;
    uint32_t BaudRate;

} Serial_Port_USART_TypeDef; */

typedef struct {
    USART_TypeDef *USARTx;

    uint32_t GPIO_RCC;  // GPIO时钟
    uint32_t USART_RCC; // USART 时钟

    // GPIO 配置
    GPIO_TypeDef *GPIO_PORT;
    uint16_t GPIO_TX_PIN;
    uint16_t GPIO_RX_PIN;

    // DMA配置
    DMA_Channel_TypeDef *DMA_RX_Channel;
    DMA_Channel_TypeDef *DMA_TX_Channel;
    uint32_t DMA_RX_IRQ;


    // NVIC配置
    uint32_t USART_IRQ;

    // USART 波特率配置
    uint32_t baudrate;

    /* DMA 缓冲区 + FIFO */
    uint8_t *dma_Rx_buf;
    uint8_t *dma_Tx_buf;
    uint16_t dma_Rx_buf_size;
    uint16_t dma_Tx_buf_size;

    // FIFO缓冲区
    uint8_t *fifo_buf;
    uint32_t fifo_size;
    volatile uint32_t fifo_head;
    volatile uint32_t fifo_tail;

} USARTx_Config_TypeDef;

#endif // !__SerialPort_USART_H__