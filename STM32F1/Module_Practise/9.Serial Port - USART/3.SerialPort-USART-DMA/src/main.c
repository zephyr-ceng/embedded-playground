
#include "stm32f10x.h"
#include "./Module/inc/SerialPort_USART_DMA.h"
#include "./Module/inc/OLED.h"
#include "./Module/inc/MyDelay.h"

#define baudrate 115200

uint8_t buffer[] = "Hello STM32";
uint8_t buf[255];
uint8_t buf_before[255];
uint8_t len = 0;

int main()
{
    OLED_Init();
    USART_Init_GPIOConfig();
    USART_Init_Config(baudrate);
    OLED_ShowString(1, 1, "stm32: ");
    while (1) {
        len = Get_ReviceData(buf);
        if (len > 0) {
            OLED_ShowString(1, 1, "stm32: ");
            OLED_ShowNum(2, 1, len, 4);
            OLED_ShowString(3, 1, (char *)buf);
            USART_TransmitData(buf, len); // 发送时使用实际的长度，而不是sizeof(buf),这个表示buf含有的字节数
            USART_TransmitData(buffer,sizeof(buffer));
        }
    }
}
