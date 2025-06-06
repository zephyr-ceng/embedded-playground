#ifndef SeralPort_H
#define SeralPort_H

void UART_Init(void); // 初始化UART
void DMA_Init_USART1(void) ; // 初始化USART1的DMA通道
void CheckBufferOverflow(void); // 检查接收缓冲区溢出并处理
void DMA_SendData_USART1(const char *data, uint16_t length); // 使用DMA发送数据
int ReadAllFromBuffer(char *buffer, int maxLength); // 从接收缓冲区读取所有数据
#endif // SeralPort_H