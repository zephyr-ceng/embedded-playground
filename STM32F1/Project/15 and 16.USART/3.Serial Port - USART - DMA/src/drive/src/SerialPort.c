#include "stm32f10x.h"
#include <string.h>
#include "stm32f10x_dma.h"

#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256
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
    DMA_InitTypeDef DMA_InitStructure;
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


    // Configure DMA1 Channel5 for USART1_RX
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)rxBuffer.buffer[0];
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = RX_BUFFER_SIZE; // Set the size of the buffer
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // Enable DMA1 Channel4 interrupt in NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE); // Enable transfer complete interrupt


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
        uint8_t nextBuffer = !txBuffer.activeBuffer; // Toggle between 0 and 1

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

// TODO: 发送函数会发送两次

// DMA发送函数
uint8_t USART_DMA_Send(char* data, uint16_t length) {
    uint8_t targetBuffer = !txBuffer.activeBuffer; // Toggle between 0 and 1
    if (txBuffer.ready[targetBuffer]) {
        targetBuffer = txBuffer.activeBuffer    ; // Use the active buffer if the target is not ready
        if (txBuffer.ready[targetBuffer]) {
            return 0; // No buffer available
        }
    }   
    if(length > TX_BUFFER_SIZE) {
        length = TX_BUFFER_SIZE; // Limit length to buffer size
    }

    memcpy(txBuffer.buffer[targetBuffer], data, length);
    txBuffer.dataLength[targetBuffer] = length;
    txBuffer.ready[targetBuffer] = 1; // Mark the buffer as ready

    DMA_Cmd(DMA1_Channel4, DISABLE); // Disable DMA before reconfiguring
    DMA_SetCurrDataCounter(DMA1_Channel4, txBuffer.dataLength[targetBuffer]); // Set the number of data items to transfer
    DMA_Cmd(DMA1_Channel4, ENABLE); // Enable DMA channel for transmission
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    return (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET) ? 1 : 0;
}
