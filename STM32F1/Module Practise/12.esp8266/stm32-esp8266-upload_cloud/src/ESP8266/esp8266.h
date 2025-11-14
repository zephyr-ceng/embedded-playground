#ifndef __esp8266_H__
#define __esp8266_H__
#include "stm32f10x.h"
#include "./USART/USART.h"

#define WIFI_SSID "WIFI_NAME" // 实际WIFI名称
#define WIFI_PASSWORD "123456789" // 实际wifi密码
#define SERVER_ADDRESS "server_address" // 服务器地址
#define SERVER_PORT 8080 // 服务器端口





void ESP8266_Init(uint32_t baudrate);
void ESP8266_SendCommand();


#endif // !__esp8266_H__