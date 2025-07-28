/****************
 *@description: Main function for STM32F10x to demonstrate UART communication and OLED display
 *@author: zephyr
 *@date: 2025-06-05 17:07:40
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/SerialPort.h"
#include "./drive/inc/OLED.h"
#include "string.h"
#include <stdio.h>

char Buffer[255];
char Msg[] = "Hello, STM32!";

uint8_t Buffer_Length = 0;

int main(void)
{
    OLED_Init();
    UART_Init();
    while (1) {
        // revice data from PC
        Buffer_Length = UART_ReadAllFromBuffer(Buffer, 255); // 从接收缓冲区读取所有内容
        OLED_ShowString(1, 1, Buffer);                       // 在OLED上显示接收到的内容
        OLED_ShowNum(2, 1, Buffer_Length, 4);                // 在OLED上显示接收的字节数

        
        UART_send_str_IT(Buffer);
        Buffer[0] = '\0'; // 清空缓冲区
        Buffer_Length = 0; // 重置缓冲区长度
        // send data to PC
        UART_send_str_IT(Msg); // 发送字符串到串口
        UART_send_str_IT("\n");
        Delay_ms(500);         // 延时1秒
    }
}
