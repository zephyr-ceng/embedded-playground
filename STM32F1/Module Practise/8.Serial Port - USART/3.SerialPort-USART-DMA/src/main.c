
#include "stm32f10x.h"
#include "./Module/inc/SerialPort_USART_DMA.h"
#include "./Module/inc/OLED.h"
#define baudrate 115200

uint8_t buffer[]    = "Hello STM32";
uint8_t RXdata[100] = {0}; // Adjust size as needed

int main()
{
    OLED_Init();
    USART_InitModule(baudrate);
    while (1) {
        USART_SendData_DMA(buffer, strlen((char *)buffer)); 
        OLED_ShowString(1, 1, (char *)RXdata); 
    }
}
