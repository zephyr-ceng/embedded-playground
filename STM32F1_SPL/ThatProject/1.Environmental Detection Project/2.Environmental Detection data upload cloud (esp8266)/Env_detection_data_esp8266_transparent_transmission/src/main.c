/****************
 *@description: esp8266 透传温湿度值
 *@brief: 使用功能有：USART、DMA-FIFO、ADC、IIC、wifi-AT指令
 *@author: zephyr
 *@date: 2025-11-21 18:19:15
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "./systick/bsp_systick.h"
#include "./Device_SerialPort/Device_USART.h"
#include "./OLED/OLED.h"
#include "./ESP8266/esp8266.h"
#include "string.h"

int main()
{
    uint16_t Send_Count              = 5;   // 发送次数
    uint16_t rx_len                  = 0;   // 接收长度
    uint8_t rx_data[BUFFER_MAX_SIZE] = {0}; // 存储接收的数据
    Device_SerialPort_USART_Init(115200);   // 串口调试
    ESP8266_Init(115200);                   // esp8266 初始化
    OLED_Init();
    SysTick_Init();
    while (1) {
        // 将接收到的数据显示OLED上，并回显
        rx_len = Device_SerialPort_ReceiveData(rx_data, rx_len);
        OLED_ShowString(1, 1, (char *)rx_data);
        while (Send_Count--) {
            Device_SerialPort_SendData(rx_data, rx_len);
            memset(rx_data, 0, rx_len); // 清空发送内容
        }

        /* 测试esp8266 透传模式 */
        ESP8266_ConnectTCP("ssid", "123456", "192.168.2.122", 8080); // 连接WIFI 和 服务器
        ESP8266_SendString("hello esp8266"); // 发送字符串
        ESP8266_SendString((char *)rx_data); // 发送字符串
        ESP8266_SendData(rx_data,rx_len); // 发送指定数据
        ESP8266_ExitTransparent();
    }
}
