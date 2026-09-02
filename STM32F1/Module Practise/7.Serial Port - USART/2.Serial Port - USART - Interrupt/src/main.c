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

uint8_t Buffer[255];
uint8_t msg[] = "Hello, STM32! it is USART!\n";

uint8_t Length = 0;

int main(void)
{
    OLED_Init();
    SerialPort_USART_Init();
    while (1) {
        Length = SerialPort_USART_ReceiveData(Buffer,255);
        if (Length > 0) {
            OLED_Clear();
            OLED_ShowNum(1,1,Length,4);
            OLED_ShowString(2,1,(char *)Buffer);
            SerialPort_USART_SendData(msg,sizeof(msg));
            SerialPort_USART_SendData(Buffer,sizeof(Buffer));
        }
    }
}
