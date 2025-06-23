#include "stm32f10x.h"
#include "./drive/inc/SerialPort.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "string.h"

uint8_t temp = 0;
char msg[] = "Hello, STM32!";

int main(void){
    OLED_Init();
    SerialPort_InitGPIO();
    USART_DMA_Init(9600);
    OLED_ShowString(1, 1, "Hello, STM32!");
    while (1)
    {
        USART_DMA_Send(msg, strlen(msg)); // Send the message
        Delay_ms(1000); // Delay for 1 second
    }
    return 0;
}