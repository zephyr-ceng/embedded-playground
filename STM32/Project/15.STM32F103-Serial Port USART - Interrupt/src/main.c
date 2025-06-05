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
char Msg[]= "Hello, STM32!";
uint8_t Buffer_Length = 0;

int main(void)
{
    OLED_Init();
    UART_Init();
    while (1) {
        // revice data from PC
        Buffer_Length = UART_ReadAllFromBuffer(Buffer,128); // 从接收缓冲区读取所有内容
        OLED_ShowString(0, 0, Buffer); // 在OLED上显示接收到的内容
        OLED_ShowNum(1, 0, Buffer_Length, 4); // 在OLED上显示接收的字节数

        // send data to PC
        UART_send_str_IT(Msg); // 发送字符串到串口
        UART_send_str_IT(Buffer); // 发送字符串到串口
        Delay_ms(500); // 延时1秒
        printf("Message is: %s\n", Msg); // 打印发送的消息
        printf("Buffer: %s, Length: %d\n", Buffer, Buffer_Length); // 打印到调试串口
        // TODO: 效果显示为：终端打印MCU发送过来的数据；OLED显示接收到的内容和字节数
    }
}
