/****************
 *@description: USART1 串口异步驱动程序
 *@author: zephyr
 *@date: 2025-06-05 11:34:16
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "string.h"

/**
 * @brief  初始化USART1
 * @note   此函数配置USART1的波特率、数据位、停止位和校验位，并设置GPIO引脚。
 * @param  null
 * @retval null
 * */
void UART_Init(void)
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
    USART_InitStructure.USART_BaudRate            = 9600;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // Enable USART1 receive interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // Enable USART1
    USART_Cmd(USART1, ENABLE);
}

// 发送和接收函数的实现

/**
 * @brief  通过USART1发送单个字节
 * @note   此函数等待发送数据寄存器空，然后发送一个字节，并等待发送完成。
 * @param  data: 要发送的字节
 * @retval null
 */
void UART_SendByte(uint8_t data)
{
    // Wait until the transmit data register is empty
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    // Send the byte
    USART_SendData(USART1, data);
    // Wait until the transmission is complete
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/**
 * @brief  通过USART1发送字符串
 * @note   此函数逐字节发送字符串，直到遇到字符串结束符'\0'，并等待发送完成。
 * @param  str: 要发送的字符串
 * @retval null
 */
void UART_SendString(char *str)
{
    while (*str) {
        UART_SendByte((uint8_t)*str);
        str++;
    }
    // Wait until the transmission of the last byte is complete
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/**
 * @brief  通过USART1接收单个字节
 * @note   此函数等待接收数据寄存器非空，然后读取接收到的字节。
 * @param  null
 * @retval 接收到的字节
 */
uint8_t UART_ReceiveByte(void)
{
    // Wait until the receive data register is not empty
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    // Read and return the received byte
    return (uint8_t)USART_ReceiveData(USART1);
}

/**
 * @brief  通过USART1接收字符串
 * @note   此函数逐字节接收数据，直到接收到停止字符或达到缓冲区大小。
 * @param  buffer: 存储接收数据的缓冲区
 * @param  maxLength: 缓冲区的最大长度
 * @param  stopChar: 停止接收的字符
 * @retval 接收到的字符串长度
 */
uint16_t UART_ReceiveString(uint8_t *buffer, uint16_t maxLength)
{
    uint16_t length = 0;
    uint8_t received;

    while (length < maxLength - 1) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

        received = USART_ReceiveData(USART1);

        if (received == '\r' || received == '\n') {
            if (length == 0)
                continue; // 跳过开头的空换行
            break;        // 结束一行
        }

        buffer[length++] = received;
    }

    buffer[length] = '\0';
    return length;
}
