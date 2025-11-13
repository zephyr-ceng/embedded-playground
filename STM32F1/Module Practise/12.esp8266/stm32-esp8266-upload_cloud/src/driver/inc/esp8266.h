#ifndef __esp8266_H__
#define __esp8266_H__
#include "stm32f10x.h"

#define USART_RX_BUFFER_SIZE 255
#define USART_TX_BUFFER_SIZE 255
#define WIFI_SSID "WIFI_NAME"
#define WIFI_PASSWORD "123456789"
#define SERVER_ADDRESS "server_address"
#define SERVER_PORT 8080

extern uint8_t RX_BUFFER[USART_RX_BUFFER_SIZE];
extern uint8_t TX_BUFFER[USART_TX_BUFFER_SIZE];




void ESP8266_Init(uint32_t baudrate);
void ESP8266_SendCommand();


#endif // !__esp8266_H__