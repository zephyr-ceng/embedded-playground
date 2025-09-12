
#include "stm32f10x.h"
#include "./Module/inc/SerialPort_USART_DMA.h"
#include "./Module/inc/OLED.h"

#define baudrate 115200

uint8_t buffer[]    = "Hello STM32";
uint8_t RXdata[1000] = {0}; // Adjust size as needed

void my_rx_handler(uint32_t *buf, uint16_t len){
    USART_TransmitData(buf,len);
}


int main()
{
    OLED_Init();
    USART_InitConfiguration(baudrate);
    USART_RegisterRxCallback(my_rx_handler);
    while (1) {
        USART_TransmitData(buffer,sizeof((char *)buffer));
    }
}
