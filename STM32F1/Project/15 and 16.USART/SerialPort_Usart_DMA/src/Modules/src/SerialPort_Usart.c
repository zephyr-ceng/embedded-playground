// #include "./Modules/inc/SerialPort_Usart.h"
#include "stm32f10x.h"

#define RxBuffer_Size 256
#define TxBuffer_Size 256

uint16_t RxBuffer[RxBuffer_Size];
uint16_t TxBuffer[TxBuffer_Size];

void Usart_Gpio_config(USART_TypeDef *USARTx, uint32_t BauRate)
{
    if (USARTx == USART1) {
        // Enable GPIO clock for USART1
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

        // Configure PA9 (TX) as alternate function push-pull
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Configure PA10 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else if (USARTx == USART2) {
        // Enable GPIO clock for USART2
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB1Periph_USART2, ENABLE);

        // Configure PA2 (TX) as alternate function push-pull
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Configure PA3 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    } else if (USARTx == USART3) {
        // Enable GPIO clock for USART3
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB1Periph_USART3, ENABLE);

        // Configure PB10 (TX) as alternate function push-pull
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        // Configure PB11 (RX) as input floating
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }

    // 初始化USART并使能
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate   = BauRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USARTx, &USART_InitStructure);
    // Enable USART idle line detection interrupt
    USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);

    USART_Cmd(USARTx, ENABLE); // Enable USART
}

void USART_DMA_Tx_Config(USART_TypeDef *USARTx)
{
    // Enable DMA clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Configure DMA for USART transmission
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)TxBuffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = TxBuffer_Size;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    
    // Initialize and enable the DMA channel
    if (USARTx == USART1) {
        DMA_Init(DMA1_Channel4, &DMA_InitStructure); // USART1 uses DMA1 Channel 4
        DMA_Cmd(DMA1_Channel4, ENABLE);
    } else if (USARTx == USART2) {
        DMA_Init(DMA1_Channel7, &DMA_InitStructure); // USART2 uses DMA1 Channel 7
        DMA_Cmd(DMA1_Channel7, ENABLE);
    } else if (USARTx == USART3) {
        DMA_Init(DMA1_Channel2, &DMA_InitStructure); // USART3 uses DMA1 Channel 2
        DMA_Cmd(DMA1_Channel2, ENABLE);
    }
    USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);
}

void USART_DMA_Rx_Config(USART_TypeDef *USARTx)
{
    // Enable DMA clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // Configure DMA for USART reception
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USARTx->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)RxBuffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = RxBuffer_Size;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;

    // Initialize and enable the DMA channel
    if (USARTx == USART1) {
        DMA_Init(DMA1_Channel5, &DMA_InitStructure); // USART1 uses DMA1 Channel 5
        DMA_Cmd(DMA1_Channel5, ENABLE);
    } else if (USARTx == USART2) {
        DMA_Init(DMA1_Channel6, &DMA_InitStructure); // USART2 uses DMA1 Channel 6
        DMA_Cmd(DMA1_Channel6, ENABLE);
    } else if (USARTx == USART3) {
        DMA_Init(DMA1_Channel3, &DMA_InitStructure); // USART3 uses DMA1 Channel 3
        DMA_Cmd(DMA1_Channel3, ENABLE);
    }

    // Enable USART to use DMA for reception
    USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);
}

void USART_NVIC_Config(USART_TypeDef *USARTx)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // Enable USART1 interrupt
    if (USARTx == USART1) {
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    } else if (USARTx == USART2) {
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    } else if (USARTx == USART3) {
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        // Clear the IDLE flag by reading the status register and data register
        (void)USART1->SR;
        (void)USART1->DR;
        uint16_t Length = RxBuffer_Size - DMA_GetCurrDataCounter(DMA1_Channel5);
        DMA_Cmd(DMA1_Channel5, DISABLE);                      // Disable DMA to process the received data
        DMA_SetCurrDataCounter(DMA1_Channel5, Length); // Reset DMA counter
        DMA_Cmd(DMA1_Channel5, ENABLE);                       // Re-enable DMA for next reception
    }
}
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
        // Clear the IDLE flag by reading the status register and data register
        (void)USART2->SR;
        (void)USART2->DR;
        uint16_t Length = RxBuffer_Size - DMA_GetCurrDataCounter(DMA1_Channel6);
        DMA_Cmd(DMA1_Channel6, DISABLE);                      // Disable DMA to process the received data
        DMA_SetCurrDataCounter(DMA1_Channel6, Length); // Reset DMA counter
        DMA_Cmd(DMA1_Channel6, ENABLE);                       // Re-enable DMA for next reception
    }
}
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) {
        // Clear the IDLE flag by reading the status register and data register
        (void)USART3->SR;
        (void)USART3->DR;
        uint16_t Length = RxBuffer_Size - DMA_GetCurrDataCounter(DMA1_Channel3);
        DMA_Cmd(DMA1_Channel3, DISABLE);                      // Disable DMA to process the received data
        DMA_SetCurrDataCounter(DMA1_Channel3, Length); // Reset DMA counter
        DMA_Cmd(DMA1_Channel3, ENABLE);                       // Re-enable DMA for next reception
    }
}

void Get_ReviceData(uint16_t *Buffer)
{
    // Copy received data to provided buffer
    for (uint16_t i = 0; i < RxBuffer_Size; i++) {
        Buffer[i] = RxBuffer[i];
    }
    
    // Clear the RxBuffer after copying
    for (uint16_t i = 0; i < RxBuffer_Size; i++) {
        RxBuffer[i] = 0;
    }
}

/**
* @brief  发送数据设置
* @param  USARTx:USART端口选择
* @param  Buffer:发送数据缓冲区指针
* @param  Length:发送数据长度
* @retval Null
* */
void Set_SendData(USART_TypeDef *USARTx,uint16_t *Buffer, uint16_t Length)
{
    if(Length> TxBuffer_Size) {
        Length = TxBuffer_Size; 
    }
    for(uint16_t i = 0; i < Length; i++) {
        TxBuffer[i] = Buffer[i];
    }
    if(USARTx == USART1) {
        DMA_Cmd(DMA1_Channel4, DISABLE); 
        DMA_SetCurrDataCounter(DMA1_Channel4, Length); 
        DMA_Cmd(DMA1_Channel4, ENABLE);
    } else if(USARTx == USART2) {
        DMA_Cmd(DMA1_Channel7, DISABLE); 
        DMA_SetCurrDataCounter(DMA1_Channel7, Length); 
        DMA_Cmd(DMA1_Channel7, ENABLE);
    } else if(USARTx == USART3) {
        DMA_Cmd(DMA1_Channel2, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Channel2, Length); 
        DMA_Cmd(DMA1_Channel2, ENABLE);
    }

}

/**
* @brief  USART DMA Init Function
* @param  USARTx:USART端口选择
* @param  BaudRate:波特率
* @param  TxBuffer:发送缓冲区指针
* @param  TxSize:发送缓冲区大小
* @note   此函数用于初始化USART的DMA传输，包括GPIO配置、DMA配置
* @retval Null
* */
void USART_DMA_Init(USART_TypeDef *USARTx, uint32_t BaudRate)
{
    // Configure GPIO for USART
    Usart_Gpio_config(USARTx, BaudRate);

    // Configure DMA for transmission
    USART_DMA_Tx_Config(USARTx);

    // Configure DMA for reception
    USART_DMA_Rx_Config(USARTx);

    USART_NVIC_Config(USARTx);
}
