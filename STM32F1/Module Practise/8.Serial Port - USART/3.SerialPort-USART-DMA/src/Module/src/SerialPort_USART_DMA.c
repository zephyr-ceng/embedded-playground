/****************
 *@description: 内存到外设的传输（调试使用单次数据传输）
 *@author: zephyr
 *@date: 2025-09-12 12:01:36
 *@version: V1.0.0
 ****************/

#include "../inc/SerialPort_USART_DMA.h"

uint32_t RX_BUFFER[USART_RX_BUFFER_SIZE];
uint32_t TX_BUFFER[USART_TX_BUFFER_SIZE];
static usart_rx_callback_t rx_callback = 0;


/**
* @brief  接收的注册回调函数
* @param  usart_rx_callback_t： 符合这个类型的函数
* @retval Null
* */
void USART_RegisterRxCallback(usart_rx_callback_t cb)
{
    rx_callback = cb;
}
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

// DMA 初始化(调试时使用单次传输模式，否则数据量巨大，导致串口助手卡顿)
void USART_Init_DMAConfig(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 使能 DMA1 时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 配置 USART1 RX 的 DMA (DMA1 Channel5)
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

    // 配置 USART1 TX 的 DMA (DMA1 Channel4)
    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)TX_BUFFER;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal; // 单次传输
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; // 禁用内存到内存
    DMA_InitStructure.DMA_BufferSize         = USART_TX_BUFFER_SIZE;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST; // 内存到外设
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium; // DMA通道优先级
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_GL4 | DMA1_FLAG_TC4 | DMA1_FLAG_HT4 | DMA1_FLAG_TE4);
    DMA_ClearFlag(DMA1_FLAG_GL5 | DMA1_FLAG_TC5 | DMA1_FLAG_HT5 | DMA1_FLAG_TE5);                                          // 清除所有Dma中断标志位
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);                  // 使能发送中断
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); // 使能 USART1 的 RX 和 TX 的 DMA 请求
    DMA_Cmd(DMA1_Channel5, ENABLE);                                  // 使能 USART1 RX 的 DMA 通道
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
/**
* @brief  USART初始化函数
* @param  baudrate: USART波特率
* @retval Null
* */
void USART_InitConfiguration(uint32_t baudrate)
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
        uint16_t received_length = USART_RX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
        // 注册回调函数
        if (rx_callback && received_length > 0) {
            rx_callback(RX_BUFFER, received_length);
        }
        // 添加其他回调函数，打印接收完成、回传、写入日志均可
    }
}

/**
* @brief  USART 数据传输函数
* @param  data: 传输的数据
* @param  len: 传输的数据长度
* @retval 1： 传输失败
* @retval 0： 传输成功
* */
uint8_t USART_TransmitData(uint16_t *data, uint16_t len)
{
    if (DMA_GetCurrDataCounter(DMA1_Channel4) != DISABLE) return 1;
    if (len == 0 || len > USART_TX_BUFFER_SIZE) return 1;
    if (data[len - 1] != '\n' && data[len - 1] != '\r') {
        if (len < USART_TX_BUFFER_SIZE) {
            data[len] = '\n'; // 添加 '\n' 作为结束符
            len++;
        }
    }
    memcpy(TX_BUFFER, data, len);
    DMA_ClearFlag(DMA1_FLAG_GL4 | DMA1_FLAG_TC4 | DMA1_FLAG_HT4 | DMA1_FLAG_TE4);
    DMA_ClearFlag(DMA1_FLAG_GL5 | DMA1_FLAG_TC5 | DMA1_FLAG_HT5 | DMA1_FLAG_TE5);
    DMA_SetCurrDataCounter(DMA1_Channel4, len);
    DMA_ITConfig(DMA1_Channel4,DMA_IT_TC ,ENABLE);
    return 0;
}

// DMA传输中断函数
void DMA1_Channel4_IRQHandler(){
    if(DMA_GetITStatus(DMA_IT_TC) != RESET){
        DMA_ClearITPendingBit(DMA_IT_TC);
        DMA_Cmd(DMA1_Channel4,DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC4);
        // 添加回调函数，如打印传输完成，或者写入日志[info]: successful
        DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,DISABLE);
    }
}
