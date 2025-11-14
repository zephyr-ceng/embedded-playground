#include "esp8266.h"


/********************************************** ESP8266函数封装 **********************************************/
void ESP8266_SendCommand(char *command)
{
    USART_SendString(command);
    USART_SendString("\r\n"); // 发送回车和换行
    Delay_ms(500); // 等待模块响应
}



// esp8266 初始化
void ESP8266_Init(uint32_t baudrate)
{
    USART_Config(baudrate);
    USART_SendString("USART 初始化成功"); // 发送到串口
    ESP8266_SendCommand("AT"); // 测试模块是否工作
    ESP8266_SendCommand("AT+CWMODE=1"); // 设置STA模式，即客户端模式

    ESP8266_SendCommand("AT+CWJAP= \"%s\"","\"%s\"",WIFI_SSID,WIFI_PASSWORD); // 连接WiFi
    ESP8266_SendCommand("AT+CIFSR"); // 获取本机IP地址
    ESP8266_SendCommand("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",SERVER_ADDRESS,SERVER_PORT); // 建立TCP连接
    ESP8266_SendCommand("AT+CIPSEND = %d",strlen("hello esp8266")); // 发送需要发送的长度
    ESP8266_SendCommand("hello esp8266"); // 发送数据
    // TODO: 发送数据完成的判定

}




