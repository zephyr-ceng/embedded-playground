#ifndef SeralPort_H
#define SeralPort_H

void UART_Init(void); // 初始化UART
void UART_SendByte(uint8_t data); // 发送单个字节
void UART_SendString(const char *str); // 发送字符串

uint8_t UART_ReadByteFromBuffer(void) ; // 从缓冲区读取单个字节
uint8_t UART_CheckBufferOverflow(void) ; // 检查缓冲区是否溢出
uint8_t UART_CheckBufferOverflow(void) ; // 检查缓冲区是否有数据
uint16_t UART_ReadAllFromBuffer(char *destBuffer, uint16_t maxLength); // 从接收缓冲区读取所有内容
void UART_Send_String(const char *str); // 发送字符串到USART1
void UART_send_str_IT(const char *str); // 发送字符串到USART1，使用中断方式
#endif // SeralPort_H