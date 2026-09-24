#ifndef SeralPort_H
#define SeralPort_H

void UART_Init(void); // 初始化UART
void UART_SendByte(uint8_t data); // 发送单个字节
void UART_SendString(const char *str); // 发送字符串
uint8_t UART_ReceiveByte(void); // 接收单个字节
uint16_t UART_ReceiveString(uint8_t *buffer, uint16_t maxLength); // 接收字符串

#endif // SeralPort_H