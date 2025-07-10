#include "stm32f10x.h"
#include "string.h"
#include "stm32f10x_dma.h"

#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256

// #define USE_DMA_DOUBLE_BUFFER

#ifdef USE_DMA_DOUBLE_BUFFER
typedef struct {
    uint8_t buffer[2][RX_BUFFER_SIZE]; // 双缓冲区
    uint8_t currentBuffer;             // 当前缓冲区索引
} serialPort_RxBuffer_t;

serialport_rxBuffer_t rxBuffer_t; // 接收缓冲区结构体
#else
uint8_t rxBuffer[RX_BUFFER_SIZE]; // 接收缓冲区
#endif
uint8_t txBuffer[TX_BUFFER_SIZE]; // 发送缓冲区
uint8_t SerialPort_Flag = 0;      // 标志位，用于指示是否有数据接收完成

/**
 * @brief  GPIO初始化函数
 * @param  baurdrate 波特率
 * @retval Null
 * */
void SerialPort_USART1_Config(uint32_t baurdrate)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Configure TX pin (PA9) as alternate function push-pull
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure RX pin (PA10) as input floating
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = baurdrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    // USART DMA enable
    USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);

    // USART 空闲中断用于接收帧尾
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    NVIC_EnableIRQ(USART1_IRQn);

    // Enable USART1 interrupt in NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART1_DMA_channel4 Send Data配置函数
 * @param  Null
 * @retval Null
 * */
void SerialPort_DMA_Channel4_Config()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)txBuffer;    // 数据缓冲区地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC; // 设置为外设到内存传输
    DMA_InitStructure.DMA_BufferSize         = TX_BUFFER_SIZE;        // 初始设置为0，将在传输过程中更新
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal; // 设置为普通模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel4, DISABLE); // 启用DMA通道4
}

void Serialport_DMA_Channel5_Config()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
#ifdef USE_DMA_DOUBLE_BUFFER
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxBuffer_t; // 数据缓冲区地址
    rxBuffer_t.currentBuffer             = 0;                    // 初始化当前缓冲区索引为0
#else
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxBuffer; // 数据缓冲区地址

#endif                                                                // DEBUG
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST; // 设置为外设到内存传输
    DMA_InitStructure.DMA_BufferSize         = RX_BUFFER_SIZE;        // 初始设置为0，将在传输过程中更新
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal; // 设置为普通模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel5, ENABLE); // 启用DMA通道5
}

void SerialPort_Init(uint32_t baudrate)
{
    SerialPort_USART1_Config(baudrate); // 初始化USART1
    SerialPort_DMA_Channel4_Config();   // 配置DMA通道4用于发送
    Serialport_DMA_Channel5_Config();   // 配置DMA通道5用于接收
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        // 清除空闲中断标志
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        uint32_t length = RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5); // 获取接收到的数据长度
#ifdef USE_DMA_DOUBLE_BUFFER
        if (rxBuffer_t[length] == '\0') { // 检查接收缓冲区是否有数据
            SerialPort_Flag = 1;          // 设置标志位，指示有数据接收完成
        }
#else
        if (rxBuffer[length] == '\0') { // 检查接收缓冲区是否有数据
            SerialPort_Flag = 1;        // 设置标志位，指示有数据接收完成
        }
#endif // !USE_DMA_DOUBLE_BUFFER
    }
}

/**
 * @brief  字符串发送函数
 * @param  data 发送的数据指针
 * @param  length 发送的数据长度
 * @retval Null
 * */
void SerialPort_SendData(char *data, uint16_t length)
{
    if (data == NULL || length == 0) {
        return; // 如果数据为空或长度为0，直接返回
    }
    if (length > TX_BUFFER_SIZE) {
        length = TX_BUFFER_SIZE; // 确保不超过缓冲区大小
    }

    memcpy(txBuffer, data, length);                     // 将数据复制到发送缓冲区
    DMA_SetCurrDataCounter(DMA1_Channel4, length);      // 设置DMA传输的数据长度
    DMA_Cmd(DMA1_Channel4, ENABLE);                     // 启用DMA通道4进行发送
    while (DMA_GetCurrDataCounter(DMA1_Channel4) != 0); // 等待DMA传输完成
    DMA_Cmd(DMA1_Channel4, DISABLE);                    // 禁用DMA通道
}

uint32_t SerialPort_ReceiveData(char *data)
{
    if (SerialPort_Flag) {
        SerialPort_Flag = 0;             // 清除标志位
        DMA_Cmd(DMA1_Channel5, DISABLE); // 禁用DMA通道
#ifdef USE_DMA_DOUBLE_BUFFER
        memcpy(data, rxBuffer_t.buffer[currentBuffer], RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5)); // 将接收到的数据复制到传入的data指针
        memset(rxBuffer_t.buffer[currentBuffer], 0, RX_BUFFER_SIZE);                                            // 清空
        uint8_t currentBuffer    = !rxBuffer_t.currentBuffer;                                                   // 获取当前缓冲区索引
        rxBuffer_t.currentBuffer = currentBuffer;                                                               // 切换到下一个缓冲
        DMA_SetCurrDataCounter(DMA1_Channel5, RX_BUFFER_SIZE);
#else
        memcpy(data, rxBuffer, RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5)); // 将接收到的数据复制到传入的data指针
        memset(rxBuffer, 0, RX_BUFFER_SIZE);                                            // 清空接收缓冲区
        DMA_SetCurrDataCounter(DMA1_Channel5, RX_BUFFER_SIZE);                          // 重置DMA计数器
#endif // USE_DMA_DOUBLE_BUFFER
    }
    return RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5); // 返回接收到的数据长度
}