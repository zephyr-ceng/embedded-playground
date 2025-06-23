/****************
 *@description: STM32F103 USART1 DMA驱动
 *@file: SerialPort.c
 *@author: zephyr
 *@date: 2025-06-17 16:11:47
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include <string.h>
#include "stm32f10x_dma.h"
#include "./drive/inc/SerialPort.h"

#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256
typedef struct {
    uint8_t buffer[2][RX_BUFFER_SIZE]; // 接收用的双缓冲区
    volatile uint8_t activeBuffer;     // 当前活动缓冲区的索引
    volatile uint16_t full[2];         // 每个缓冲区中的数据长度
} SerialPortRxBuffer;

typedef struct {
    uint8_t buffer[2][TX_BUFFER_SIZE]; // 双缓冲区用于传输
    volatile uint8_t activeBuffer;     // 当前活动缓冲区的索引
    volatile uint8_t ready[2];         // 缓冲区是否准备好
    uint16_t dataLength[2];            // 每个缓冲区中的数据长度
} SerialPortTxBuffer;


// 初始化缓冲区
SerialPortRxBuffer rxBuffer;
SerialPortTxBuffer txBuffer;

/**
 * @brief  初始化GPIO引脚用于USART1
 * @note   USART1使用PA9作为TX引脚，PA10作为RX引脚
 * @param  Null
 * @retval Null
 * */
void USART_GPIO_Init(void)
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

void USART_Config(uint32_t baudrate)
{
    // Enable USART1 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // Configure USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

void USART_DMA_channel4_Init()
{
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

    // Initialize TX buffer
    txBuffer.activeBuffer = 0;
    txBuffer.ready[0]     = 0;
    txBuffer.ready[1]     = 0;
}

void USART_DMA_channel5_Init()
{
    // Configure DMA1 Channel5 for USART1_RX
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)rxBuffer.buffer[0];
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = RX_BUFFER_SIZE; // Set to maximum size
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular; // Use circular mode for continuous reception
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel5, ENABLE);                // Enable DMA1 Channel5
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE); // Enable USART1 DMA receiver

    // Initialize RX buffer
    rxBuffer.activeBuffer = 0;
    rxBuffer.full[0]      = 0;
    rxBuffer.full[1]      = 0;
}

/**
 * @brief  使用DMA发送数据
 * @note   该函数将数据复制到活动缓冲区，并启动DMA传输
 * @param  data: 指向要发送的数据的指针
 * @param  length: 要发送的数据长度
 * @retval lenth: 实际发送的数据长度
 */
TransmissionStatus USART_DMA_Send(char *data, uint16_t length)
{
    if (data == NULL || length == 0) {
        return TRANSMISSION_EMPTY; // 如果数据为空或长度为0，直接返回
    } else {
        DMA_Cmd(DMA1_Channel4, DISABLE); // 禁用DMA通道4以重新配置

        // 检查当前活动缓冲区是否已准备好,并将数据复制到缓冲区
        uint8_t targetBuffer = txBuffer.activeBuffer; // 获取当前活动缓冲区
        length = (length > TX_BUFFER_SIZE) ? TX_BUFFER_SIZE : length; // 限制长度不超过缓冲区大小
        memcpy(txBuffer.buffer[targetBuffer], data, length); // 复制数据到活动缓冲区
        txBuffer.dataLength[targetBuffer] = length;          // 设置数据长度
        txBuffer.ready[targetBuffer]      = 1;               // 标记缓冲区已准备好

        // 配置DMA传输
        DMA_Cmd(DMA1_Channel4, ENABLE); // 启用DMA通道4
        DMA_SetCurrDataCounter(DMA1_Channel4, length); // 设置DMA传输的数据长度
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); // 启用USART的DMA传输
        txBuffer.activeBuffer = !txBuffer.activeBuffer; // 切换到另一个缓冲区
        txBuffer.ready[!txBuffer.activeBuffer] = 0; // 将非活动缓冲区标记为未准备好

        // 超时处理
/*         uint32_t timeout = 1000000; // 设置超时时间
        while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET) {
            if (timeout-- == 0) {
                // 超时处理
                DMA_Cmd(DMA1_Channel4, DISABLE);                // 禁用DMA
                USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE); // 禁用USART的DMA传输
                return TRANSMISSION_TIMEOUT; // 退出函数
            }
        } */
        DMA_ClearFlag(DMA1_FLAG_TC4); // 清除传输完成标志
        return TRANSMISSION_SUCCESS; // 返回成功状态
    }
}

/**
 * @brief  从接收缓冲区读取数据
 * @note   检查当前活动缓冲区是否有数据可读，如果有则读取数据
 * @param  buffer: 存储读取数据的目标缓冲区
 * @param  maxLength: 目标缓冲区的最大长度
 * @retval 实际读取的数据长度
 */
uint16_t USART_DMA_Read(char *buffer, uint16_t maxLength)
{
    uint8_t activeBuffer   = rxBuffer.activeBuffer; // 获取当前活动缓冲区
    uint8_t inactiveBuffer = !activeBuffer;         // 获取非活动缓冲区

    if (rxBuffer.full[inactiveBuffer]) {
        // 如果非活动缓冲区有数据，切换缓冲区
        rxBuffer.activeBuffer         = inactiveBuffer;
        rxBuffer.full[inactiveBuffer] = 0; // 标记非活动缓冲区为空
        activeBuffer                  = inactiveBuffer;
    }

    uint16_t length = (rxBuffer.full[activeBuffer] > maxLength) ? maxLength : rxBuffer.full[activeBuffer]; // 获取当前缓冲区的数据长度
    memcpy(buffer, rxBuffer.buffer[activeBuffer], length);                                                 // 复制数据到目标缓冲区
    rxBuffer.full[activeBuffer] = 0;                                                                           // 标记当前缓冲区为空

    return length; // 返回实际读取的数据长度
}

/**
* @brief  初始化USART1和DMA
* @note   该函数初始化GPIO引脚，配置USART1，并初始化DMA通道4和5
* @param  baudrate: USART1的波特率
* @retval Null
* */
void USART_DMA_Init(uint32_t baudrate)
{
    USART_GPIO_Init();     // 初始化GPIO引脚
    USART_Config(baudrate);    // 配置USART1
    USART_DMA_channel4_Init(); // 初始化DMA通道4用于发送
    USART_DMA_channel5_Init(); // 初始化DMA通道5用于接收
}