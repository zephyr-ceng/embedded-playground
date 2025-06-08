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
uint16_t Buffer_Length = 0;


int main(void)
{
    OLED_Init();
    UART_Init();
    OLED_ShowString(1, 1, "Receive Data:"); // Show a static message on OLED
    // OLED_ShowString(2,1,Msg);
    while (1) {
        Buffer_Length = UART_ReceiveString(Buffer, 255); 
        Delay_ms(500);
        OLED_ShowString(2, 1, Buffer); // Display received data on OLED

        // Echo the received data back to the sender
        UART_SendString(Buffer); 
        UART_SendString("\n"); 
        UART_SendString(Msg); 
        Delay_ms(500); 
        printf("Message sent: %s\n", Msg); 
    }
}
