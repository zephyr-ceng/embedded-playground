/****************
 *@description: 内存到外设的传输
 *@author: zephyr
 *@date: 2025-09-12 12:01:36
 *@version: V1.0.0
 ****************/

#include "../inc/SerialPort_USART_DMA.h"

uint8_t RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t TX_BUFFER[USART_TX_BUFFER_SIZE];
uint16_t received_length = 0; // 获取数据长度
/**
 * @brief  接收的注册回调函数
 * @param  usart_rx_callback_t： 符合这个类型的函数
 * @retval Null
 * */

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
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; // 使能RX和TX
    USART_Init(USART1, &USART_InitStructure);
    USART_ClearFlag(USART1, USART_IT_IDLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 配合DMA使用可以处理不定长数据

    // DMA初始化 RX
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)RX_BUFFER; // 内存地址
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;     // 循环接收
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;       // 禁用内存到内存
    DMA_InitStructure.DMA_BufferSize         = USART_RX_BUFFER_SIZE;  // 传输初值
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC; // 外设到内存
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设传输初始值
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 固定外设，地址不变
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;         // 中优先级
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // 初始化 TX
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)TX_BUFFER; // 内存地址
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;       // 循环发送
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;       // 禁用内存到内存
    DMA_InitStructure.DMA_BufferSize         = USART_TX_BUFFER_SIZE;  // 传输初值
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST; // 内存到外设
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 传输初始值
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 固定外设，地址不变
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;         // 中优先级
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // NVIC初始化
    NVIC_InitTypeDef NVIC_Init_Structure;
    NVIC_Init_Structure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_Init_Structure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init_Structure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init_Structure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_Init(&NVIC_Init_Structure);

    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); // 使能rx tx
    USART_Cmd(USART1, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
}

/********************************************** 功能封装 **********************************************/
// USART 接收中断服务程序
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        DMA_Cmd(DMA1_Channel5, DISABLE); // 关闭 DMA 通道
        (void)USART1->SR;                // 读取 SR 寄存器清除中断标志
        (void)USART1->DR;                // 读取 DR 寄存器清除中断标志
        received_length = USART_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
        // 可添加添加日志记录
        DMA_SetCurrDataCounter(DMA1_Channel5, USART_RX_BUFFER_SIZE);
        DMA_Cmd(DMA1_Channel5, ENABLE);
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
    }
}

/**
* @brief  返回实际接收到的数据的长度
* @param  Buf: 用来存储实际接收到的数据
* @retval len: 接收的数据长度
* */
uint16_t Get_ReviceData(uint8_t *Buf)
{
    if (received_length > 0)
        uint16_t len = received_length;
    memset(Buf, 0, USART_RX_BUFFER_SIZE); // 清除接收数据的Buf
    memcpy(Buf, RX_BUFFER, len);
    memset(RX_BUFFER, 0, USART_RX_BUFFER_SIZE); // 清除RX_BUFFER 防止上次的数据遗留
    received_length = 0;                        // 清除传输长度
    return len;
}


/**
* @brief  发送指定数据
* @param  data: 实际发送的数据
* @param  length: 发送的数据长度
* @retval NULl
* */
void USART_TransmitData(uint8_t *data, uint16_t length)
{
    if (length > 0 && data != NULL) {
        DMA_Cmd(DMA1_Channel4, DISABLE);
        memcpy(TX_BUFFER, data, length);
        DMA_SetCurrDataCounter(DMA1_Channel4, length);
        DMA_Cmd(DMA1_Channel4, ENABLE);
    }
}