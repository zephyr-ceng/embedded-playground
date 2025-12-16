/****************
 *@description: UART通信和OLED显示
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
char Msg[] = "Hello, STM32!";

uint16_t Buffer_Length = 0;

int main(void)
{
    OLED_Init();
    UART_Init();
    OLED_ShowString(1, 1, "Receive Data:"); // Show a static message on OLED
    // OLED_ShowString(2,1,Msg);
    while (1) {
        UART_SendString(Msg);
        Delay_ms(1000);
        Buffer_Length = UART_ReceiveString(Buffer, 255);
        OLED_ShowString(2, 1, (char *)Buffer); // Display received data on OLED
        // Delay_ms(500);
        UART_SendString((char *)Buffer);
        UART_SendString("\n");
    }
}
