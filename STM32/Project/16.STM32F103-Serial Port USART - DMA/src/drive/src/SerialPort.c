#include "stm32f10x.h"

typedef struct {
    uint8_t buffer[2][RX_BUFFER_SIZE]; // Double buffer for reception
    volatile uint8_t activeBuffer;     // Index of the active buffer
    volatile uint16_t full[2];   // Length of data in each buffer
} SerialPortRxBuffer;

typedef struct {
    uint8_t buffer[2][TX_BUFFER_SIZE];          // Double buffer for transmission
    volatile uint8_t activeBuffer;   // Index of the active buffer
    volatile uint8_t ready[2];
    uint16_t dataLength[2]; // Length of data in each buffer
} SerialPortTxBuffer;

SerialPortRxBuffer rxBuffer;
SerialPortTxBuffer txBuffer;

void SerialPort_InitGPIO(void)
{
    // Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

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
}

void USART_DMA_Init(uint32_t baurdrate)
{
    // Enable USART1 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

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

    // Enable DMA1 clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Configure DMA1 Channel4 for USART1_TX
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)txBuffer.buffer[0];
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0; // Initially set to 0, will be updated during transmission
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    // Enable DMA1 Channel4 interrupt in NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // Enable transfer complete interrupt

    // Configure DMA1 Channel5 for USART1_RX
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)rxBuffer.buffer[0];
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = RX_BUFFER_SIZE; // Set the size of the buffer
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // Enable DMA1 Channel5
    DMA_Cmd(DMA1_Channel5, ENABLE);

    // Enable USART1 DMA receiver
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    // Initialize RX buffer
    rxBuffer.activeBuffer = 0;
    rxBuffer.full[0] = 0;
    rxBuffer.full[1] = 0;

    // Initialize TX buffer
    txBuffer.activeBuffer = 0;
    txBuffer.ready[0] = 0;
    txBuffer.ready[1] = 0;

}

/**
* @brief  DMA1 Channel4 interrupt handler for USART1 transmission
* @param  Null
* @retval Null
* */
void DMA1_Channel4_IRQHandler(void)
{
    // Check if the transfer complete interrupt flag is set
    if (DMA_GetITStatus(DMA1_IT_TC4))
    {
        // Clear the interrupt flag
        DMA_ClearITPendingBit(DMA1_IT_TC4);
        
        // Mark the current buffer as ready
        txBuffer.ready[txBuffer.activeBuffer] = 0;

        // Switch to the other buffer if it is ready
        // uint8_t nextBuffer = (txBuffer.activeBuffer + 1) % 2;
        uint8_t nextBuffer != txBuffer.activeBuffer; // Toggle between 0 and 1

        if (txBuffer.ready[nextBuffer])
        {
            txBuffer.activeBuffer = nextBuffer;

            // Update DMA memory base address and buffer size
            DMA_Cmd(DMA1_Channel4, DISABLE);
            DMA1_Channel4->CMAR = (uint32_t)txBuffer.buffer[nextBuffer];
            DMA1_Channel4->CNDTR = txBuffer.dataLength[nextBuffer];
            DMA_Cmd(DMA1_Channel4, ENABLE);
            USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
        }
    }
}

// TODO: 需要修改接收函数

// DMA发送函数
uint8_t USART_DMA_Send(uint8_t* data, uint16_t length) {
    // 查找空闲的发送缓冲区
    uint8_t targetBuffer = !dmaTx.activeBuffer;
    
    // 如果目标缓冲区忙，则使用当前活动缓冲区的备用
    if (dmaTx.ready[targetBuffer]) {
        targetBuffer = dmaTx.activeBuffer;
        if (dmaTx.ready[targetBuffer]) {
            // 两个缓冲区都忙，无法发送
            return 0;
        }
    }
    
    // 检查数据长度有效性
    if (length > TX_BUFFER_SIZE) {
        length = TX_BUFFER_SIZE;
    }
    
    // 复制数据到缓冲区
    memcpy(dmaTx.buffer[targetBuffer], data, length);
    dmaTx.lengths[targetBuffer] = length;
    dmaTx.ready[targetBuffer] = 1;
    
    // 如果当前没有DMA传输，立即启动
    if (!DMA_GetCmdStatus(DMA1_Channel4)) {
        dmaTx.activeBuffer = targetBuffer;
        
        DMA1_Channel4->CMAR = (uint32_t)dmaTx.buffer[targetBuffer];
        DMA1_Channel4->CNDTR = length;
        DMA_Cmd(DMA1_Channel4, ENABLE);
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    }
    
    return 1;
}

// 接收缓冲区处理函数（在主循环中定期调用）
void USART_DMA_ProcessReceived(void) {
    static uint16_t lastPos[2] = {0, 0};
    uint8_t processBuffer = dmaRx.activeBuffer;
    
    // 计算当前DMA位置
    uint16_t currentPos = RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
    
    // 检查缓冲区是否回绕
    if (currentPos < lastPos[processBuffer]) {
        // 处理从lastPos到缓冲区末尾的数据
        uint16_t length = RX_BUFFER_SIZE - lastPos[processBuffer];
        if (length > 0) {
            // 处理数据 (dmaRx.buffer[processBuffer] + lastPos[processBuffer], length)
            // 这里添加实际的数据处理代码
        }
        lastPos[processBuffer] = 0;
    }
    
    // 处理剩余数据
    if (currentPos > lastPos[processBuffer]) {
        uint16_t length = currentPos - lastPos[processBuffer];
        if (length > 0) {
            // 处理数据 (dmaRx.buffer[processBuffer] + lastPos[processBuffer], length)
            // 这里添加实际的数据处理代码
        }
        lastPos[processBuffer] = currentPos;
    }
    
    // 检查是否缓冲区满（可选）
    if (currentPos == RX_BUFFER_SIZE - 1) {
        dmaRx.full[processBuffer] = 1;
    }
}

// 切换接收缓冲区（在数据处理完成后调用）
void USART_DMA_SwitchRxBuffer(void) {
    // 禁用DMA接收
    DMA_Cmd(DMA1_Channel5, DISABLE);
    
    // 切换活动缓冲区
    uint8_t newBuffer = !dmaRx.activeBuffer;
    
    // 配置新缓冲区
    DMA1_Channel5->CMAR = (uint32_t)dmaRx.buffer[newBuffer];
    DMA1_Channel5->CNDTR = RX_BUFFER_SIZE;
    
    // 重置状态
    dmaRx.full[newBuffer] = 0;
    dmaRx.activeBuffer = newBuffer;
    
    // 重新使能DMA
    DMA_Cmd(DMA1_Channel5, ENABLE);
}
