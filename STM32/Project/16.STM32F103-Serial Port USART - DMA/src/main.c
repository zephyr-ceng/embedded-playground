/****************
 *@description: Main function for STM32F10x to demonstrate UART and DMA communication and OLED display
 *@author: zephyr
 *@date: 2025-06-05 17:07:40
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/SerialPort.h"
#include "./drive/inc/OLED.h"


char Buffer[255];
char Msg[]= "Hello, STM32!";
uint16_t Buffer_Length = 0;

int main(void)
{
    OLED_Init();
    USART1_DMA_Config();
    USART1_Config();
    OLED_ShowString(1, 1, "STM32 UART DMA");
    while (1) {
        Buffer_Length = Read_Rx_Buffer((uint8_t*)Buffer, sizeof(Buffer) - 1);
        if (Buffer_Length > 0) {
            Buffer[Buffer_Length] = '\0'; // 确保字符串结束
            OLED_ShowString(2, 1, Buffer); // 显示接收到的数据
            DMA_Send((uint8_t*)Msg, sizeof(Msg) - 1); // 发送消息
            DMA_Send(Buffer,sizeof(Buffer) -1); // 发送消息
        }
        Delay_ms(100); // 延时100毫秒
    }
}
