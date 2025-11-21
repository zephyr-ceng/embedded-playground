#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"

#define DEVICE_USART_NUM 3

#if (DEVICE_USART_NUM == 1)

#define DEVICE_TX_GPIO_PORT     GPIOA                /* 对应GPIO端口 */
#define DEVICE_TX_GPIO_CLK_PORT RCC_APB2Periph_GPIOA /* 对应GPIO端口时钟位 */
#define DEVICE_TX_GPIO_PIN      GPIO_Pin_9           /* 对应PIN脚 */
#define DEVICE_RX_GPIO_PORT     GPIOA                /* 对应GPIO端口 */
#define DEVICE_RX_GPIO_CLK_PORT RCC_APB2Periph_GPIOA /* 对应GPIO端口时钟位 */
#define DEVICE_RX_GPIO_PIN      GPIO_Pin_10          /* 对应PIN脚 */

#define DEVICE_USARTX           USART1                 /* 对应串口号 */
#define DEVICE_USARTX_CLK_PORT  RCC_APB2Periph_USART1  /* 对应串口外设时钟位 */
#define DEVICE_APBXCLKCMD       RCC_APB2PeriphClockCmd /* 对应串口外设时钟 */

#define DEVICE_IRQ              USART1_IRQn       /* 对应串口中断号 */
#define DEVICE_IRQHANDLER       USART1_IRQHandler /* 对应串口中断处理函数 */

#define DEVICE_RX_CHANNEL       DMA1_Channel4
#define DEVICE_TX_CHANNEL       DMA1_Channel5
#define DEVICE_DMA_FLAG         DMA1_FLAG_TC5

#elif (DEVICE_USART_NUM == 2)

#define DEVICE_TX_GPIO_PORT     GPIOA                /* 对应GPIO端口 */
#define DEVICE_TX_GPIO_CLK_PORT RCC_APB2Periph_GPIOA /* 对应GPIO端口时钟位 */
#define DEVICE_TX_GPIO_PIN      GPIO_Pin_2           /* 对应PIN脚 */
#define DEVICE_RX_GPIO_PORT     GPIOA                /* 对应GPIO端口 */
#define DEVICE_RX_GPIO_CLK_PORT RCC_APB2Periph_GPIOA /* 对应GPIO端口时钟位 */
#define DEVICE_RX_GPIO_PIN      GPIO_Pin_3           /* 对应PIN脚 */

#define DEVICE_USARTX           USART2                 /* 对应串口号 */
#define DEVICE_USARTX_CLK_PORT  RCC_APB1Periph_USART2  /* 对应串口外设时钟位 */
#define DEVICE_APBXCLKCMD       RCC_APB1PeriphClockCmd /* 对应串口外设时钟 */

#define DEVICE_IRQ              USART2_IRQn       /* 对应串口中断号 */
#define DEVICE_IRQHANDLER       USART2_IRQHandler /* 对应串口中断处理函数 */

#define DEVICE_RX_CHANNEL       DMA1_Channel6
#define DEVICE_TX_CHANNEL       DMA1_Channel7
#define DEVICE_DMA_FLAG         DMA1_FLAG_TC7

#elif (DEVICE_USART_NUM == 3)

#define DEVICE_TX_GPIO_PORT     GPIOB                /* 对应GPIO端口 */
#define DEVICE_TX_GPIO_CLK_PORT RCC_APB2Periph_GPIOB /* 对应GPIO端口时钟位 */
#define DEVICE_TX_GPIO_PIN      GPIO_Pin_10          /* 对应PIN脚 */
#define DEVICE_RX_GPIO_PORT     GPIOB                /* 对应GPIO端口 */
#define DEVICE_RX_GPIO_CLK_PORT RCC_APB2Periph_GPIOB /* 对应GPIO端口时钟位 */
#define DEVICE_RX_GPIO_PIN      GPIO_Pin_11          /* 对应PIN脚 */

#define DEVICE_USARTX           USART3                 /* 对应串口号 */
#define DEVICE_USARTX_CLK_PORT  RCC_APB1Periph_USART3  /* 对应串口外设时钟位 */
#define DEVICE_APBXCLKCMD       RCC_APB1PeriphClockCmd /* 对应串口外设时钟 */

#define DEVICE_IRQ              USART3_IRQn       /* 对应串口中断号 */
#define DEVICE_IRQHANDLER       USART3_IRQHandler /* 对应串口中断处理函数 */

#define DEVICE_RX_CHANNEL       DMA1_Channel2
#define DEVICE_TX_CHANNEL       DMA1_Channel3
#define DEVICE_DMA_FLAG         DMA1_FLAG_TC3

#endif

#define BUFFER_SIZE     1024 // 收发字节的大小
#define BUFFER_MAX_SIZE 2048 // FIFO缓存大小

uint8_t dma_rx_buf[BUFFER_SIZE];
uint8_t dma_tx_buf[BUFFER_SIZE];
typedef struct {
    uint8_t Buffer[BUFFER_SIZE];
    uint32_t len;
    uint32_t read_flag;
} DEVICE_DataTypedef;

// FIFO定义
typedef struct {
    uint8_t buf[BUFFER_SIZE]; // FIFO缓存数组
    uint16_t read_idx;        // 读指针
    uint16_t write_idx;       // 写指针
    uint16_t len;             // 当前有效数据长度
} DEVICE_FIFO_Typedef;

extern DEVICE_DataTypedef device_receive;
extern DEVICE_FIFO_Typedef fifo;
extern uint8_t dma_rx_Maxbuf[BUFFER_MAX_SIZE]; // DMA接收缓冲区

// 实际函数封装
void Device_SerialPort_USART_Init(uint32_t BAUDRATE);
uint16_t Device_SerialPort_ReceiveData(uint8_t *data, uint16_t length);
void Device_SerialPort_SendData(uint8_t *data, uint16_t length);

#endif // !__USART_H__
