#include "USART.h"
#include "./Delay/MyDelay.h"
#include "stdio.h"
#include "string.h"

uint8_t RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t TX_BUFFER[USART_TX_BUFFER_SIZE];
uint16_t received_length = 0; // 获取数据长度

void Serial_USART_Config(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // GPIO初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode  = USART1_TX_MODE; // TX 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode  = USART1_RX_MODE; // RX 浮空输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART初始化
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_InitStructure.USART_BaudRate            = baudrate;
    USART_InitStructure.USART_WordLength          = TIM_DMABurstLength_8Bytes;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // DMA RX配置
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
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

    // DMA TX配置
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

    // NVIC配置
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能 DMA 和 USART
    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
    USART_ITConfig(USART1, USART_FLAG_IDLE, ENABLE); // 空闲中断
    DMA_Cmd(DMA1_Channel5, ENABLE);                  // 接收DMA
    USART_Cmd(USART1, ENABLE);
}

/********************************************** USART 功能封装 **********************************************/

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
 * @brief  接收在255字节中的不定长数据
 * @param  Buf: 用来存储实际接收到的数据
 * @retval len: 接收的数据长度
 * */
uint16_t Serial_USART_ReceiveData(uint8_t *Buf)
{
    uint16_t len = 0;
    if (received_length > 0)
        len = received_length;
    memset(Buf, 0, USART_RX_BUFFER_SIZE); // 清除接收数据的Buf
    memcpy(Buf, RX_BUFFER, len);
    memset(RX_BUFFER, 0, USART_RX_BUFFER_SIZE); // 清除RX_BUFFER 防止上次的数据遗留
    received_length = 0;                        // 清除传输长度
    return len;
}

// 发送字符串函数
void Serial_USART_SendString(char *str)
{
    uint16_t len;
    if (str == NULL) return;
    len = strlen(str);
    if (len == 0 || len > USART_TX_BUFFER_SIZE) return;

    DMA_Cmd(DMA1_Channel4, DISABLE);
    memcpy(TX_BUFFER, str, len);
    DMA_SetCurrDataCounter(DMA1_Channel4, len);
    DMA_Cmd(DMA1_Channel4, ENABLE);
}

// 发送通用型数据
void Serial_USART_SendData(uint8_t *data,uint16_t length){
    if(length == 0 || data ==NULL || length > USART_TX_BUFFER_SIZE) return;
    DMA_Cmd(DMA1_Channel4,DISABLE);
    memcpy(TX_BUFFER,data,length);
    DMA_SetCurrDataCounter(DMA1_Channel4,length);
    DMA_Cmd(DMA1_Channel4,ENABLE);
}
