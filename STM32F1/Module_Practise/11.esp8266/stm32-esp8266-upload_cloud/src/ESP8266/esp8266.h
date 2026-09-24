#ifndef __esp8266_H__
#define __esp8266_H__
#include "stm32f10x.h"
#include "./USART/USART.h"
#include "./Delay/MyDelay.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"


// ESP8266 引脚配置（片选和复位）
#define ESP8266_GPIO_PORT GPIOB
#define ESP8266_PD_PIN GPIO_Pin_8
#define ESP8266_PD_MODE GPIO_Mode_Out_PP
#define ESP8266_RST_PIN GPIO_Pin_9

/********************************************** ESP8266 的引脚控制 **********************************************/
#define ESP8266_PD_H() GPIO_SetBits(ESP8266_GPIO_PORT,ESP8266_PD_PIN) 
#define ESP8266_PD_L() GPIO_ResetBits(ESP8266_GPIO_PORT,ESP8266_PD_PIN)

#define ESP8266_RST_H() GPIO_SetBits(ESP8266_GPIO_PORT,ESP8266_RST_PIN)
#define ESP8266_RST_L() GPIO_ResetBits(ESP8266_GPIO_PORT,ESP8266_RST_PIN)
/********************************************** ESP8266函数声明 **********************************************/
void ESP8266_Init(uint32_t baudrate);
void ESP8266_Reset();
bool ESP8266_SendCommand(char *command, char *reply1, char *reply2, uint16_t waitTime);
bool ESP8266_ConnectTCP(char *ssid, char *password, char *serverAddr,uint16_t serverPort);
bool ESP8266_SendData(char *data);

#endif // !__esp8266_H__