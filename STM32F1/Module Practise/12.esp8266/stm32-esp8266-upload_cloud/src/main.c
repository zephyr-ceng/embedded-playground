/****************
 *@description: esp8266 数据上传cloud
 *@@brief: 本次使用ESP8266模块通过AT指令连接WiFi，并将数据上传至云平台。
 *@author: zephyr
 *@date: 2025-11-13 15:29:55
 *@version: V1.0.0
****************/
/* 系统头文件 */
#include "stm32f10x.h"
/* 外部引用文件 */
#include "./ESP8266/esp8266.h"
// #include "./USART/USART.h"
#include "./Delay/MyDelay.h"
#include "./OLED/OLED.h"

void main()
{
    /* 模块初始化 */
    ESP8266_Init(115200);
    ESP8266_Reset();
    OLED_Init();
    /* 模块测试 */
    ESP8266_ConnectTCP("WIFI_NAME", "123456789", "server_address", 8080);
    ESP8266_SendData("Hello, Cloud!");
    /* 主循环 */
    while (1)
    {
        
    }
}
