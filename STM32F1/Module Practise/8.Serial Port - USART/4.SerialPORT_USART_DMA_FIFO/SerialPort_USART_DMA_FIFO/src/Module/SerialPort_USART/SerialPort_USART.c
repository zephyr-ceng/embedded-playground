#include "./SerialPort_USART.h"

/********************************************** 基本定义 **********************************************/
uint32_t recv_length;
// uint32_t head;
// uint32_t tail;
// uint32_t free;

/* === DMA 物理缓冲区 === */
uint8_t usart1_dma_rx[256];
uint8_t usart2_dma_rx[256];
uint8_t usart3_dma_rx[256];

/* === FIFO 缓冲区 === */
uint8_t usart1_fifo[2048];
uint8_t usart2_fifo[2048];
uint8_t usart3_fifo[2048];

USARTx_Config_TypeDef uart_cfg[3] =
    {
        /* =====================================
         * USART1 - index 0
         * ===================================== */
        {
            .USARTx    = USART1,
            .GPIO_RCC  = RCC_APB2Periph_GPIOA,
            .USART_RCC = RCC_APB2Periph_USART1,

            .GPIO_PORT   = GPIOA,
            .GPIO_TX_PIN = GPIO_Pin_9,
            .GPIO_RX_PIN = GPIO_Pin_10,

            .DMA_RX_Channel = DMA1_Channel5,
            .DMA_TX_Channel = DMA1_Channel4,
            .DMA_RX_IRQ     = DMA1_Channel5_IRQn,

            .USART_IRQ = USART1_IRQn,
            .baudrate  = USART_BAUDRATE,

            .dma_Rx_buf      = dma_rx_bufs[0],
            .dma_Tx_buf      = dma_tx_bufs[0],
            .dma_Rx_buf_size = 256,
            .dma_Tx_buf_size = 256,

            .fifo_buf  = fifo_bufs[0],
            .fifo_size = 512,
            .fifo_head = 0,
            .fifo_tail = 0,
        },

        /* =====================================
         * USART2 - index 1
         * ===================================== */
        {
            .USARTx    = USART2,
            .GPIO_RCC  = RCC_APB2Periph_GPIOA,
            .USART_RCC = RCC_APB1Periph_USART2,

            .GPIO_PORT   = GPIOA,
            .GPIO_TX_PIN = GPIO_Pin_2,
            .GPIO_RX_PIN = GPIO_Pin_3,

            .DMA_RX_Channel = DMA1_Channel6,
            .DMA_TX_Channel = DMA1_Channel7,
            .DMA_RX_IRQ     = DMA1_Channel6_IRQn,

            .USART_IRQ = USART2_IRQn,
            .baudrate  = USART_BAUDRATE,

            .dma_Rx_buf      = dma_rx_bufs[1],
            .dma_Tx_buf      = dma_tx_bufs[1],
            .dma_Rx_buf_size = 256,
            .dma_Tx_buf_size = 256,

            .fifo_buf  = fifo_bufs[1],
            .fifo_size = 512,
            .fifo_head = 0,
            .fifo_tail = 0,
        },

        /* =====================================
         * USART3 - index 2
         * ===================================== */
        {
            .USARTx    = USART3,
            .GPIO_RCC  = RCC_APB2Periph_GPIOB,
            .USART_RCC = RCC_APB1Periph_USART3,

            .GPIO_PORT   = GPIOB,
            .GPIO_TX_PIN = GPIO_Pin_10,
            .GPIO_RX_PIN = GPIO_Pin_11,

            .DMA_RX_Channel = DMA1_Channel3,
            .DMA_TX_Channel = DMA1_Channel2,
            .DMA_RX_IRQ     = DMA1_Channel3_IRQn,

            .USART_IRQ = USART3_IRQn,
            .baudrate  = USART_BAUDRATE,

            .dma_Rx_buf      = dma_rx_bufs[2],
            .dma_Tx_buf      = dma_tx_bufs[2],
            .dma_Rx_buf_size = 256,
            .dma_Tx_buf_size = 256,

            .fifo_buf  = fifo_bufs[2],
            .fifo_size = 512,
            .fifo_head = 0,
            .fifo_tail = 0,
        }};

uint8_t USART_NUM = sizeof(uart_cfg) / sizeof(uart_cfg[0]);

// 时钟+GPIO配置
void SerialPort_USART_GPIO_Config(USARTx_Config_TypeDef *cfg)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(cfg->GPIO_RCC | RCC_APB2Periph_AFIO, ENABLE);

    /* ---- GPIO ---- */
    GPIO_InitStructure.GPIO_Pin   = cfg->GPIO_TX_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(cfg->GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = cfg->GPIO_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(cfg->GPIO_PORT, &GPIO_InitStructure);
}

// USART配置
void SerialPort_USART_Mode_Config(USARTx_Config_TypeDef *cfg)
{
    USART_InitTypeDef USART_InitStructure;
    if (cfg->USARTx == USART1)
        RCC_APB2PeriphClockCmd(cfg->USART_RCC, ENABLE);
    else
        RCC_APB1PeriphClockCmd(cfg->USART_RCC, ENABLE);

    USART_InitStructure.USART_BaudRate            = cfg->baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    // 使能控制
    USART_Init(cfg->USARTx, &USART_InitStructure);
    USART_ITConfig(cfg->USARTx, USART_IT_IDLE, ENABLE); // 使能IDLE中断
    USART_Cmd(cfg->USARTx, ENABLE);
}

// DMA配置
void SerialPort_USART_DMA_Config(USARTx_Config_TypeDef *cfg)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // tx
    DMA_DeInit(cfg->DMA_TX_Channel);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&cfg->USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)&cfg->dma_Tx_buf;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST; // 内存到外设
    DMA_InitStructure.DMA_BufferSize         = cfg->dma_Tx_buf_size;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Enable; // 外设地址递增
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;    // 内存不递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(cfg->DMA_TX_Channel, &DMA_InitStructure);

    // rx
    DMA_DeInit(cfg->DMA_RX_Channel);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&cfg->USARTx->DR;  // USART 数据寄存器地址
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)cfg->dma_buf;      // DMA缓冲区
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;       // 外设到内存 DST= 内存到外设
    DMA_InitStructure.DMA_BufferSize         = cfg->dma_Rx_buf_size;        // 缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不变
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设传输为字节
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存传输为字节
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;           // 循环模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;           // 高优先级
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;             // 禁用内存到内存传输
    DMA_Init(cfg->DMA_RX_CH, &DMA_InitStructure);

    USART_DMACmd(cfg->USARTx, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); // 使能USART DMA接收请求
    DMA_Cmd(cfg->DMA_RX_CH, ENABLE);                                      // 使能DMA接收通道
}

// NVIC信号
void SerialPort_USART_NVIC_Config(USARTx_Config_TypeDef *cfg)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = cfg->USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
}

/********************************************** 功能封装 **********************************************/
void SerialPort_USARTx_Init(USARTx_Config_TypeDef *cfg)
{
    SerialPort_USART_GPIO_Config(cfg);
    SerialPort_USART_Mode_Config(cfg);
    SerialPort_USART_DMA_Config(cfg);
    SerialPort_USART_NVIC_Config(cfg);
}

void USART1_IRQHandler(void)
{
    extern USARTx_Config_t uart_cfg[];
    USARTx_IRQHandler(&uart_cfg[0]);
}

void USART2_IRQHandler(void)
{
    extern USARTx_Config_t uart_cfg[];
    USARTx_IRQHandler(&uart_cfg[1]);
}

void USART3_IRQHandler(void)
{
    extern USARTx_Config_t uart_cfg[];
    USARTx_IRQHandler(&uart_cfg[2]);
}

void USARTx_IRQHandler(USARTx_Config_TypeDef *cfg)
{
    if (USART_GetFlagStatus(cfg->USARTx, USART_IT_IDLE) != RESET) {
        USART_ClearITPendingBit(cfg->USARTx, USART_IT_IDLE);
        DMA_Cmd(DMA1_Channel5, DISABLE); // 关闭 DMA 通道
        (void)USART1->SR;                // 读取 SR 寄存器清除中断标志
        (void)USART1->DR;                // 读取 DR 寄存器清除中断标志

        // 数据处理
        recv_length = cfg->dma_Rx_buf_size - DMA_GetCurrDataCounter(cfg->DMA_RX_Channel);
    }
}

void USART_RX_DataProcess(USARTx_Config_TypeDef *cfg)
{
    /* === 复制到 FIFO === */
    uint32_t head = cfg->fifo_head; // 当前写入的位置
    uint32_t tail = cfg->fifo_tail; // 当前读取位置
    uint32_t free;                  // 剩余可写入的长度区域

    // 判定当前读取位置和写入的位置，如果读取在前，剩余的长度为：总长-(未读取长度)-1； 读取在当前写入之后：总长=读取的长度区域-写入的长度区域-1
    // 未读取长度 = 当前写入 - 当前读取
    if (head >= tail)
        free = cfg->fifo_size - (head - tail) - 1;
    else
        free = (tail - head) - 1;

    if (recv_length > free) recv_length = free; // 单次写入长度超过FIFO的大小(当前可写入的部分free)，保留只能放入的部分

    uint32_t first = cfg->fifo_size - head; // 计算第一次写入后剩余的长度
    if (first > recv_length) first = recv_length; // 最大写入为recv_length

    // TODO: 计算仍需理解
    memcpy(&cfg->fifo_buf[head], cfg->dma_buf, first); // 将DMA接收到的内容写入FIFO
    head = (head + first) % cfg->fifo_size;

    if (first < recv_length) {
        uint32_t second = recv_length - first;
        memcpy(&cfg->fifo_buf[head], &cfg->dma_buf[first], second);
        head = (head + second) % cfg->fifo_size;
    }

    cfg->fifo_head = head;
}
