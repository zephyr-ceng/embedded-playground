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

char Buffer[100];
char Msg[]= "Hello, STM32!";
uint8_t Buffer_Length = 0;

int main(void)
{
    OLED_Init();
    UART_Init();
    while (1) {
        // receive a message, show it on OLED, and send a message back to PC
        Buffer_Length = UART_ReceiveString(Buffer, strlen(Buffer), '\0'); // receive a string until newline character
        OLED_ShowString(0, 0, Buffer); // display the received string on OLED
        OLED_ShowNum(1,0, Buffer_Length, 4); // display the length of the received string on OLED
        Delay_ms(500); // wait for 0.5 second
        UART_SendString(Buffer); // send the received string back to PC

        // send a message to PC
        UART_SendString(Msg); // send a predefined message
        UART_SendString(Buffer); // send the received string again
        Delay_ms(500); // wait for 0.5 second
        printf("Message sent: %s\n", Msg); // print the message to console for debugging

        // TODO: OLED显示接收的数据，终端和串口工具显示MCU发送的数据
    }
}
