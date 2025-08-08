/****************
 *@description: Usart Driver for Stm32F1 about DMA
 *@author: zephyr
 *@date: 2025-07-30 10:50:30
 *@version: V1.0.0
****************/

#include "./BSP_Drive/inc/MyDelay.h"
#include "./Modules/inc/SerialPort_Usart.h"
#include "string.h"

char msg[] = "Hello, USART with DMA!\r\n";
uint16_t ReviceData[256]; // Buffer to store received data

int main(void)
{
    USART_DMA_Init(USART1, 115200); // Initialize USART1 with DMA
    Get_ReviceData(ReviceData); // Get received data, 接收的数据会自动通过DMA存储到ReviceData中，不需要手动读取
    while (1)
    {
/*         if (ReviceData)
        {
            // 在OLED或其他显示设备上显示接收到的数据
        } */
        Set_SendData(USART1, (uint16_t *)msg, strlen(msg)); // Send data via USART1
    }
    return 0;
}


// TODO: 串口待测试验证