
#include "stm32f10x.h"
#include "./Module/inc/SerialPort_USART_DMA.h"
#include "./Module/inc/OLED.h"
#include "./Module/inc/MyDelay.h"

#define baudrate 115200

uint8_t buffer[] = "Hello STM32";
uint8_t buf[255];
uint8_t buf_before[255];
uint8_t len;

int main()
{
    OLED_Init();
    USART_Init_GPIOConfig();
    USART_Init_Config(baudrate);
    OLED_ShowString(1, 1, "stm32: ");
    len = Get_ReviceData(buf);
    OLED_ShowString(3, 1, (char *)buf);
    memcpy(buf_before, buf, len);
    while (1) {
        len = Get_ReviceData(buf);
        // 数据一致性对比，有更新就刷新OLED 重新显示数据
        // TODO:数据过长，会影响新的数据更新
        for (size_t i = 0; i < len; i++) {
            if (buf_before[i] != buf[i]) {
                OLED_Clear();
                OLED_ShowString(1, 1, "stm32: ");
                memcpy(buf_before, buf, len);
            }
        }

        OLED_ShowNum(2, 1, len, 6);
        OLED_ShowString(3, 1, (char *)buf);
        USART_TransmitData(buf, strlen((char *)buf)); // 发送时使用实际的长度，而不是sizeof(buf),这个表示buf含有的字节数
    }
}


