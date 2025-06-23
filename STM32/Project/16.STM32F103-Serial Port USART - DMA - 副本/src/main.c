#include "stm32f10x.h"
#include "./drive/inc/SerialPort.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "string.h"

char msg[] = "Hello, STM32 with DMA USART!";
char buffer[256]; // 用于接收数据的缓冲区


int main(void) {
    OLED_Init();        // 初始化OLED
    USART_DMA_Init(9600); // 初始化USART1，波特率为115200
    OLED_ShowString(1, 1, "Hello USART"); // 在OLED上显示初始化状态
    while (1) {
        // 一直发送msg
        USART_DMA_Send(msg, strlen(msg)); // 发送消息
        // OLED_ShowString(1, 1, "Sending:"); // 在OLED上显示发送状态

        // 接收串口发送的数据
        memset(buffer, 0, sizeof(buffer)); // 清空接收缓冲区
        USART_DMA_Read(buffer, sizeof(buffer)); // 读取数据
        OLED_ShowString(2, 1, buffer); // 在OLED上显示接收到的数据
        Delay_ms(1000); // 延时1秒
    }
}