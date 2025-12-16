/****************
 *@description: USART1 串口-中断收发
 *@author: zephyr
 *@date: 2025-06-05 11:34:16
 *@version: V1.0.0
 ****************/
#include "../inc/SerialPort.h"

#define RX_BUFFER_SIZE 128 // 接收缓冲区大小

uint8_t rxBuffer[RX_BUFFER_SIZE]; // 接收缓冲区
uint8_t rxBuffer_flag = 0;
uint8_t received;
uint8_t len = 0;

/**
 * @brief  初始化USART1
 * @note   此函数配置USART1的波特率、数据位、停止位和校验位，并设置GPIO引脚。
 * @param  null
 * @retval null
 * */
void SerialPort_USART_Init(void)
{
    // Enable clocks for GPIOA and USART1
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

    // Configure USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // Configure NVIC for USART1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable USART1 receive interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  USART1中断处理程序
 * @note   处理接收到的数据，包括单字节和字符串，并处理缓冲区溢出。
 * @param  null
 * @retval null
 */
void USART1_IRQHandler(void)
{
    // 检查是否为接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // 读取接收到的数据
        received = USART_ReceiveData(USART1);
        // USART_SendData(USART1, received);
        rxBuffer[len++] = received;
        if (received == '\n' || received == '\r')
            rxBuffer_flag = 1;
    }
}

uint8_t SerialPort_USART_ReceiveData(uint8_t *data, uint16_t length)
{

    if (data == NULL || length == 0) return 0;
    uint16_t temp = len;
    if (rxBuffer_flag) {
        memcpy(data, rxBuffer, len); // OLED 无法显示换行符
        memset(rxBuffer, 0, RX_BUFFER_SIZE);
        rxBuffer_flag = 0;
        len           = 0;
        return temp;
    }
    return 0;
}

void SerialPort_USART_SendData(uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0) return;
    for (uint8_t i = 0; i < length; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); // 表示txe还在传输中，等待传输完成
        USART_SendData(USART1, data[i]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // 表示等待tc发送完成
}
