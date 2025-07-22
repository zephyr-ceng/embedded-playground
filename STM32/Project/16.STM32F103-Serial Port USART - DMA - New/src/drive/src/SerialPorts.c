#include "stm32f10x.h"
#include "string.h"
#include "stm32f10x_dma.h"

#define TX_BUFFER_SIZE        256
#define RX_BUFFER_SIZE        256

#define RCC_APB2Periph_GPIOx  RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_USARTx RCC_APB2Periph_USART1
#define GPIOx                 GPIOA
#define GPIOx_TX_Pin          GPIO_Pin_9
#define GPIOx_RX_Pin          GPIO_Pin_10
#define DMAx_Channelx         DMA1_Channel4
#define DMAx_Channely         DMA1_Channel5
#define USARTx                USART1
#define baudrate              115200

#define RCC_AHBPeriph_DMAx    RCC_AHBPeriph_DMA1
#define USARTx_IRQHandler     USART1_IRQHandler

uint8_t txBuffer[TX_BUFFER_SIZE]; // 发送缓冲区
uint8_t rxBuffer[RX_BUFFER_SIZE]; // 接收缓冲区

void SerialPort_USART1_Config(uint32_t baudrate)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx | RCC_APB2Periph_USARTx, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Configure TX pin (PA9) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin   = GPIOx_TX_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    // Configure RX pin (PA10) as input floating
    GPIO_InitStructure.GPIO_Pin  = GPIOx_RX_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    // Configure USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);
    USART_Cmd(USARTx, ENABLE);

    // Enable DMA for USART
    USART_DMACmd(USARTx, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);

    // Enable IDLE interrupt for frame end detection
    USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);
    NVIC_EnableIRQ(USARTx_IRQn); // Enable USART interrupt in NVIC

    // Enable USART1 interrupt in NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void SerialPort_DMA_TX_Channel_Config()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMAx, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMAx_Channelx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)txBuffer;    // 数据缓冲区地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC; // 设置为内存到外设传输
    DMA_InitStructure.DMA_BufferSize         = TX_BUFFER_SIZE;        // 初始设置为0，将在传输过程中更新
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal; // 设置为普通模式
    \DMA_InitStructure.DMA_Priority          = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMAx_Channelx, &DMA_InitStructure);
    DMA_Cmd(DMAx_Channelx, ENABLE); // 启用DMA通道4
}


void SerialPort_sendData(uint8_t* data, uint16_t length)
{
    if (data == NULL || length == 0) {
        return; // 如果数据为空或长度为0，直接返回
    }
    if (length > TX_BUFFER_SIZE) {
        length = TX_BUFFER_SIZE; // 确保不超过缓冲区大小
    }

    memcpy(txBuffer, data, length);                     // 将数据复制到发送缓冲区
    DMA_SetCurrDataCounter(DMAx_Channelx, length);      // 设置DMA传输的数据长度
    DMA_Cmd(DMAx_Channelx, ENABLE);                     // 启用DMA通道4进行发送
    while (DMA_GetCurrDataCounter(DMAx_Channelx) != 0); // 等待DMA传输完成
    DMA_Cmd(DMAx_Channelx, DISABLE);                    // 禁用DMA通道
}