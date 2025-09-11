#include "../inc/SerialPort_USART_DMA.h"

#define USART_RX_BUFFER_SIZE 256
#define USART_TX_BUFFER_SIZE 256

uint32_t RX_BUFFER[USART_RX_BUFFER_SIZE];
uint32_t TX_BUFFER[USART_TX_BUFFER_SIZE];
uint8_t ReceiveFlag = 0;
uint8_t TransmitFlag = 0;
/******************************* 初始化 *******************************/

// GPIO 初始化
void USART_Init_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能 GPIOA 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置 USART1 TX (PA9) 为复用推挽输出
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 USART1 RX (PA10) 为浮空输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// USART 初始化
void USART_Init_Config(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStructure;

    // 使能 USART1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // 配置 USART1 参数
    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    // 使能空闲中断，用于接收完成标志
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 配合DMA使用可以处理不定长数据

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// DMA 初始化
void USART_Init_DMAConfig(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 使能 DMA1 时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 配置 USART1 RX 的 DMA (DMA1 Channel5)
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)RX_BUFFER;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC; // 外设到内存
    DMA_InitStructure.DMA_BufferSize         = USART_RX_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular; // 循环模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // 配置 USART1 TX 的 DMA (DMA1 Channel4)
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)TX_BUFFER;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST; // 内存到外设
    DMA_InitStructure.DMA_BufferSize         = USART_TX_BUFFER_SIZE;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // 使能接收完成中断

    // 使能 USART1 的 RX 和 TX 的 DMA 请求
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);

    // 使能 USART1 RX 的 DMA 通道
    DMA_Cmd(DMA1_Channel5, ENABLE);
}

// NVIC 初始化
void USART_Init_NVICConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // 配置 NVIC 优先级分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // 配置 USART1 中断
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 主优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1; // 次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 配置 DMA1 Channel4 (USART1 TX) 中断
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************************* 功能实现 *******************************/
void USART_InitModule(uint32_t baudrate)
{
    USART_Init_GPIOConfig();
    USART_Init_Config(baudrate);
    USART_Init_DMAConfig();
    USART_Init_NVICConfig();
}

// USART 接收中断服务程序
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        (void)USART1->SR;                // 读取 SR 寄存器清除中断标志
        (void)USART1->DR;                // 读取 DR 寄存器清除中断标志
        DMA_Cmd(DMA1_Channel5, DISABLE); // 关闭 DMA 通道
        // uint16_t received_length = USART_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
        ReceiveFlag = 1;
    }
}

/**
* @brief  接收数据
* @param  data:存储接收的数据
* @retval NULL
* */
void USART_ReceiveData_DMA(uint8_t *data, uint8_t len)
{
    if (ReceiveFlag) {
        uint16_t received_length = USART_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
        if (received_length <= len && (data[received_length - 1] == '\n' || data[received_length - 1] == '\r')) {
            memcpy(data, RX_BUFFER, received_length);
            memset(RX_BUFFER,0,sizeof(RX_BUFFER));
            DMA_SetCurrDataCounter(DMA1_Channel5, USART_RX_BUFFER_SIZE); // 重设数据计数器
            DMA_Cmd(DMA1_Channel5, ENABLE);                              // 重新开启 DMA 通道
            ReceiveFlag = 0;
        }
        // TODO: 接收数据不完整和接收数据长度超SIZE限制
    }
}

// 发送中断
void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_TC4); // 清除中断标志
        DMA_Cmd(DMA1_Channel4, DISABLE);    // 关闭 DMA 通道
        TransmitFlag = 0;
    }
}

/**
* @brief  发送数据
* @param  data: 要发送的数据
* @retval NULL
* */
void USART_SendData_DMA(uint8_t *data, uint8_t length)
{
    if (TransmitFlag == 0) {
        // 确保len小于等于USART_TX_BUFFER_SIZE
        uint8_t len = length < USART_TX_BUFFER_SIZE ? length : USART_TX_BUFFER_SIZE;
        if (data[len - 1] != '\n' && data[len - 1] != '\r') {
            if (len < USART_TX_BUFFER_SIZE) {
                data[len] = '\n'; // 添加 '\n' 作为结束符
                len++;
            }
        }
        memcpy(TX_BUFFER, data, len);
        DMA_SetCurrDataCounter(DMA1_Channel4, len); // 设置发送长度
        DMA_Cmd(DMA1_Channel4, ENABLE);
        TransmitFlag = 1;  // DMA发送通道启动状态
    }
}
