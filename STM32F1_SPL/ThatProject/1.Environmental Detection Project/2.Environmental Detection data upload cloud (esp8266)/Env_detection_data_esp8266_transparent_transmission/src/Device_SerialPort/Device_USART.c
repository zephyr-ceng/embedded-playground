#include "Device_USART.h"

uint8_t dma_rx_Maxbuf[BUFFER_MAX_SIZE] = {0}; // DMA接收缓冲区
DEVICE_DataTypedef device_receive;
DEVICE_FIFO_Typedef fifo;

/********************************************** 基础功能配置 **********************************************/
// GPIO配置
void Device_SerialPort_GPIO_Config()
{
    GPIO_InitTypeDef gpio_initstruct = {0};
    /* 开启 DEVICE 相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(DEVICE_TX_GPIO_CLK_PORT, ENABLE);

    /*选择要控制的GPIO引脚、设置GPIO模式为 推挽复用、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio_initstruct.GPIO_Pin   = DEVICE_TX_GPIO_PIN;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEVICE_TX_GPIO_PORT, &gpio_initstruct);

    /* 开启 DEVICE 相关的GPIO外设/端口时钟 */
    RCC_APB2PeriphClockCmd(DEVICE_RX_GPIO_CLK_PORT, ENABLE);

    /*选择要控制的GPIO引脚、设置GPIO模式为 上拉输入/浮空输入、设置GPIO速率为50MHz*/
    gpio_initstruct.GPIO_Mode  = GPIO_Mode_IPU;
    gpio_initstruct.GPIO_Pin   = DEVICE_RX_GPIO_PIN;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEVICE_RX_GPIO_PORT, &gpio_initstruct);
}

// USART配置
void Device_SerialPort_USART_Config(uint32_t BAUDRATE)
{
    /* 定义一个 USART 结构体 */
    USART_InitTypeDef usart_initstruct = {0};

    /* 开启 DEVICE 相关的GPIO外设/端口时钟 */
    DEVICE_APBXCLKCMD(DEVICE_USARTX_CLK_PORT, ENABLE);

    /* 配置串口的工作参数 */
    usart_initstruct.USART_BaudRate            = BAUDRATE;                       // 配置波特率
    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 配置硬件流控制
    usart_initstruct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // 配置工作模式
    usart_initstruct.USART_Parity              = USART_Parity_No;                // 配置校验位
    usart_initstruct.USART_StopBits            = USART_StopBits_1;               // 配置停止位
    usart_initstruct.USART_WordLength          = USART_WordLength_8b;            // 配置帧数据字长

    USART_Init(DEVICE_USARTX, &usart_initstruct);
#if 0
    USART_ITConfig(DEVICE_USARTX, USART_IT_RXNE, ENABLE); // 开启串口数据接收中断
#endif
    USART_ITConfig(DEVICE_USARTX, USART_IT_IDLE, ENABLE); // 开启串口数据空闲中断
    USART_Cmd(DEVICE_USARTX, ENABLE);
}
// NVIC配置
void Device_SerialPort_NVIC_Config()
{
    NVIC_InitTypeDef nvic_initstruct = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    nvic_initstruct.NVIC_IRQChannel                   = DEVICE_IRQ; /* 配置中断源 */
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 1;          /* 配置抢占优先级 */
    nvic_initstruct.NVIC_IRQChannelSubPriority        = 0;          /* 配置子优先级 */
    nvic_initstruct.NVIC_IRQChannelCmd                = ENABLE;     /* 使能配置中断通道 */
    NVIC_Init(&nvic_initstruct);
}
// DMA配置
void Device_SerialPort_DMA_Config()
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // rx
    DMA_DeInit(DEVICE_RX_CHANNEL);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;       // 外设地址：USART1数据寄存器
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)dma_rx_buf;        // 内存地址：DMA接收缓冲区
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralSRC;       // 外设->内存
    DMA_InitStruct.DMA_BufferSize         = BUFFER_SIZE;                 // 缓冲区大小
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 字节对齐
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Circular;   // 循环模式
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Medium; // 中等优先级
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;     // 非内存到内存
    DMA_Init(DEVICE_RX_CHANNEL, &DMA_InitStruct);
    // tx
    DMA_DeInit(DEVICE_TX_CHANNEL);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;       // 外设地址：USART1数据寄存器
    DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)dma_tx_buf;        // 内存地址：DMA接收缓冲区
    DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralDST;       // 内存->外设
    DMA_InitStruct.DMA_BufferSize         = BUFFER_SIZE;                 // 缓冲区大小
    DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
    DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 字节对齐
    DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode               = DMA_Mode_Circular;   // 循环模式
    DMA_InitStruct.DMA_Priority           = DMA_Priority_Medium; // 中等优先级
    DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;     // 非内存到内存
    DMA_Init(DEVICE_RX_CHANNEL, &DMA_InitStruct);

    // 使能
    USART_DMACmd(DEVICE_USARTX, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DEVICE_RX_CHANNEL, ENABLE);
}

// 将数据写入FIFO
static void Device_SerialPort_FIFO_Write(uint8_t *data, uint16_t length)
{
    uint16_t i;
    // 空指针校验
    if (length == 0 || data == NULL)
        return;

    // 循环写入数据至fifo
    for (i = 0; i < length; i++) {
        if (fifo.len > BUFFER_SIZE) {
            break;
        }
        fifo.buf[fifo.write_idx] = data[i];
        fifo.write_idx           = (fifo.write_idx + 1) / BUFFER_SIZE;
        fifo.len++;
    }
}


/**
* @brief  中断处理函数, 当触发闲时中断时，对DMA收到的数据进行拷贝
* @param  NULL
* @retval NULL
* */
void DEVICE_IRQHANDLER(void)
{

#if 0
    uint8_t data_temp = NULL;
    if (USART_GetITStatus(DEVICE_USARTX, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(DEVICE_USARTX,USART_IT_RXNE);
        data_temp = USART_ReceiveData(DEVICE_USARTX);
        if ((device_receive.len < BUFFER_SIZE - 1) && device_receive.read_flag == 0) {
            device_receive.Buffer[device_receive.len] = data_temp;
            device_receive.len++;
        }
        if (device_receive.len == BUFFER_SIZE - 1) // 如果接满数据包结束
        {
            device_receive.buffer[device_receive.len] = '\0'; // 插入字符串结尾标志
            device_receive.read_flag                  = 1;    // 正在读取数据标志置1，暂不接收
        }

    }
#endif

#if 1
    if (USART_GetITStatus(DEVICE_USARTX, USART_IT_IDLE) != RESET) {
        USART_ClearFlag(DEVICE_USARTX, USART_IT_IDLE);
        DMA_Cmd(DEVICE_RX_CHANNEL, DISABLE);
        uint16_t len = BUFFER_SIZE - DMA_GetCurrDataCounter(DEVICE_RX_CHANNEL);
        if (len > 0 && len < BUFFER_SIZE)
            Device_SerialPort_FIFO_Write(dma_rx_buf, len);
        DMA_SetCurrDataCounter(DEVICE_RX_CHANNEL, BUFFER_SIZE);
        DMA_Cmd(DEVICE_RX_CHANNEL, ENABLE);
    }
#endif
}

// 读取FIFO缓冲区数据
uint16_t Device_SerialPort_FIFO_Read(uint8_t *data, uint16_t length)
{
    uint16_t i;
    if (length == 0 || data == NULL)
        return 0;
    uint16_t read_len = (length > fifo.len) ? fifo.len : length;
    for (i = 0; i < read_len; i++) {
        data[i]       = fifo.buf[fifo.read_idx];
        fifo.read_idx = (fifo.read_idx + 1) % BUFFER_SIZE;
        fifo.len--;
    }
    return read_len;
}

// 清空fifo
void Device_SerialPort_FIFO_Clear()
{
    fifo.read_idx  = 0;
    fifo.write_idx = 0;
    fifo.len       = 0;
}

/********************************************** 具体发送和接收函数封装 **********************************************/
// device SerialPort init
void Device_SerialPort_USART_Init(uint32_t BAUDRATE)
{
    Device_SerialPort_GPIO_Config();
    Device_SerialPort_USART_Config(BAUDRATE);
    Device_SerialPort_DMA_Config();
    Device_SerialPort_NVIC_Config();
}

uint16_t Device_SerialPort_ReceiveData(uint8_t *data, uint16_t length)
{
    return Device_SerialPort_FIFO_Read(data, length);
}

void Device_SerialPort_SendData(uint8_t *data, uint16_t length)
{
    // 空指针判定
    if (length == 0 || data == NULL) return;
    // 等待上一次发送完成后再进行发送
    while (DMA_GetFlagStatus(DEVICE_DMA_FLAG) == RESET);
    DMA_ClearFlag(DEVICE_DMA_FLAG);
    memcpy(dma_tx_buf, data, length);
    DMA_Cmd(DEVICE_TX_CHANNEL, DISABLE);
    DMA_SetCurrDataCounter(DEVICE_TX_CHANNEL, length);
    DMA_Cmd(DEVICE_TX_CHANNEL, ENABLE);
}
