/****************
 *@description: esp32 wifi 透传模式
 *@brief: 主要配置步骤如下：
 ** 1.配置串口通信
 ** 2.使用串口发送AT指令，结尾添加\r\n;
 ** 3.AT指令参考：
 ** AT	测试模块是否在线	OK
 ** AT+CWMODE=1	设置为 STA 模式（连接路由器）	OK
 ** AT+CWJAP="WiFi 名","密码"	连接本地 WiFi	WIFI CONNECTED
 ** AT+CIPMUX=0	关闭多连接模式（单连接）	OK
 ** AT+CIPSTART="TCP","服务器 IP", 端口	连接 TCP 服务器（如电脑本地 IP:8080）	CONNECT OK
 ** AT+CIPMODE=1	开启透传模式	OK
 ** AT+CIPSEND	进入透传发送模式（之后数据直接透传）	>
 *@author: zephyr
 *@date: 2025-11-21 15:40:15
 *@version: V1.0.0
****************/

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
#define ESP8266_PD_PIN    GPIO_Pin_8
#define ESP8266_PD_MODE   GPIO_Mode_Out_PP
#define ESP8266_RST_PIN   GPIO_Pin_9
#define ESP8266_CMD_BUFFER_SIZE 256

/********************************************** ESP8266 的引脚控制 **********************************************/
#define ESP8266_PD_H()  GPIO_SetBits(ESP8266_GPIO_PORT, ESP8266_PD_PIN)
#define ESP8266_PD_L()  GPIO_ResetBits(ESP8266_GPIO_PORT, ESP8266_PD_PIN)

#define ESP8266_RST_H() GPIO_SetBits(ESP8266_GPIO_PORT, ESP8266_RST_PIN)
#define ESP8266_RST_L() GPIO_ResetBits(ESP8266_GPIO_PORT, ESP8266_RST_PIN)
/********************************************** ESP8266函数声明 **********************************************/

void ESP8266_Init(uint32_t baudrate);
void ESP8266_Reset();
bool ESP8266_SendCommand(char *command, char *reply1, char *reply2, uint16_t waitTime);
bool ESP8266_ConnectTCP(char *ssid, char *password, char *serverAddr, uint16_t serverPort);
bool ESP8266_SendString(char *data);
bool ESP8266_SendData(uint8_t *data, uint16_t length);
void ESP8266_ExitTransparent(void);
#endif // !__esp8266_H__