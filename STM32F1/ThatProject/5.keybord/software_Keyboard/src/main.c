/****************
 *@description: 使用USART+DMA和FIFO缓冲区进行数据传输
 *@@brief: 涉及DMA的数据传输配置、 FIFO的数据存储及读取的概念
 *@author: zephyr
 *@date: 2025-11-20 18:43:27
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "./systick/bsp_systick.h"
#include "./USART/USART.h"
#include "./OLED/OLED.h"
#include "string.h"

int main()
{
    uint16_t Send_Count              = 0;   // 发送次数
    uint16_t rx_len                  = 0;   // 接收长度
    uint8_t rx_data[BUFFER_MAX_SIZE] = {0}; // 存储接收的数据

    SerialPort_USART_Init(115200);
    OLED_Init();
    SysTick_Init();
    while (1) {
        // 将接收到的数据显示OLED上，并回显
        rx_len = SerialPort_ReceiveData(rx_data, rx_len);
        OLED_ShowString(1, 1, (char *)rx_data);
        while (Send_Count--) {
            SerialPort_SendData(rx_data, rx_len);
            memset(rx_data, 0, rx_len); // 清空发送内容
        }
    }
}

// TODO: 可测试USART1,2,3
