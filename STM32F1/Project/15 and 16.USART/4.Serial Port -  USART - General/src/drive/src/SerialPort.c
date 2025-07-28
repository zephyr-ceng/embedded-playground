#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "string.h"

typedef struct {
    USART_TypeDef *USARTx;             // USART外设
    DMA_Channel_TypeDef *DMAx_channel; // 接收DMA通道
    IRQn_Type USART_IRQn;              // 中断号
    uint8_t *rxBuffer;                 // 接收缓冲区
    uint16_t bufSize;                  // 缓冲区大小
    uint8_t *txBuffer;          // 发送缓冲区
    uint16_t txBufSize;       // 发送缓冲区大小
    // UsartDMARXCallback onReceive;      // 接收回调函数
    void (*onReceive)(uint8_t *data, uint16_t len); // 接收回调函数
} USARTDMAHandle;

static inline void UsartDMA_GPIO_Config(USART_TypeDef *USARTx)
{
    switch (USARTx) {
        case USART1:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

            gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
            gpio.GPIO_Speed = GPIO_Speed_50MHz;
            gpio.GPIO_Pin   = GPIO_Pin_9; // TX
            GPIO_Init(GPIOA, &gpio);

            gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            gpio.GPIO_Pin  = GPIO_Pin_10; // RX
            GPIO_Init(GPIOA, &gpio);
            break;
        case USART2:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART2, ENABLE);
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

            gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
            gpio.GPIO_Speed = GPIO_Speed_50MHz;
            gpio.GPIO_Pin   = GPIO_Pin_2; // TX
            GPIO_Init(GPIOA, &gpio);

            gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            gpio.GPIO_Pin  = GPIO_Pin_3; // RX
            GPIO_Init(GPIOA, &gpio);
            break;

        case USART3:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

            gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
            gpio.GPIO_Speed = GPIO_Speed_50MHz;
            gpio.GPIO_Pin   = GPIO_Pin_10; // TX
            GPIO_Init(GPIOB, &gpio);

            gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            gpio.GPIO_Pin  = GPIO_Pin_11; // RX
            GPIO_Init(GPIOB, &gpio);
            break;
    }
}

// 初始化USART和DMA
void UsartDMA_Init(USARTDMAHandle *handel, uint32_t baudrate)
{
    UsartDMA_GPIO_Config(handel->USARTx); // 配置GPIO
    // USART配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = baudrate; // 波特率
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(handel->USARTx, &USART_InitStructure);
    USART_Cmd(handel->USARTx, ENABLE);

    // DMA配置
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(handel->DMAx_channel);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&handel->USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)handel->rxBuffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = handel->bufSize; // 设置为0，后续通过回调函数设置
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; // 禁用内存到内存传输
    DMA_Init(handel->DMAx_channel, &DMA_InitStructure);



    // 启用DMA通道
    DMA_Cmd(handel->DMAx_channel, ENABLE);
    USART_DMACmd(handel->USARTx, USART_DMAReq_Rx, ENABLE); // 启用USART DMA接收

    // NVIC配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = handel->USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;      // 优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;      // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE; // 使能
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(handel->USARTx, USART_IT_IDLE, ENABLE); // 使能空闲中断
    NVIC_EnableIRQ(handel->USART_IRQn);                    // 启用USART中断
}

// 发送数据(轮询)
void UsartDMA_Send(USARTDMAHandle *handel, uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        while (USART_GetFlagStatus(handel->USARTx, USART_FLAG_TXE) == RESET); // 等待发送缓冲区空
        USART_SendData(handel->USARTx, data[i]);                              // 发送数据
    }
    while (USART_GetFlagStatus(handel->USARTx, USART_FLAG_TC) == RESET); // 等待发送完成
}

// DMA接收中断处理函数
void UsartDMA_RX_IRQHandler(USARTDMAHandle *handel)
{
    if (USART_GetITStatus(handel->USARTx, USART_IT_IDLE) != RESET) {
        USART_ClearITPendingBit(handel->USARTx, USART_IT_IDLE); // 清除空闲中断标志
        // (void)handel->USARTx->SR; // 读取SR寄存器以清除中断
        // (void)handel->USARTx->DR; // 读取DR寄存器以清除中断

        uint16_t length = handel->bufSize - DMA_GetCurrDataCounter(handel->DMAx_channel); // 获取接收到的数据长度
        if (length > 0 && handel->onReceive) {
            handel->onReceive(handel->rxBuffer, length); // 调用接收回调函数
        }
        DMA_Cmd(handel->DMAx_channel, DISABLE);                        // 禁用DMA通道
        DMA_SetCurrDataCounter(handel->DMAx_channel, handel->bufSize); // 重置DMA计数器
        DMA_Cmd(handel->DMAx_channel, ENABLE);                         // 重新启用DMA通道
    }
}