#include "esp8266.h"

void ESP8266_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 片选引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ESP8266_PD_PIN;
    GPIO_InitStructure.GPIO_Mode  = ESP8266_PD_MODE;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ESP8266_GPIO_PORT, &GPIO_InitStructure);

    // 复位引脚
    GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
    GPIO_Init(ESP8266_GPIO_PORT, &GPIO_InitStructure);
}

/********************************************** ESP8266函数封装 **********************************************/

// esp8266 初始化 将复位脚和片选使能脚拉高
void ESP8266_Init(uint32_t baudrate)
{
    Serial_USART_Config(baudrate);
    ESP8266_Config();
    ESP8266_PD_H();
    ESP8266_RST_H();
}

// esp8266 复位
void ESP8266_Reset()
{
    ESP8266_RST_L();
    Delay_ms(50);
    ESP8266_RST_H();
    Delay_ms(500);
}

// 命令发送及返回指令确认
bool ESP8266_SendCommand(char *command, char *reply1, char *reply2, uint16_t waitTime)
{
    uint16_t len;
    uint8_t buffer[256];
    len = Serial_USART_ReceiveData(buffer);
    Serial_USART_SendString(command);
    Serial_USART_SendString("\r\n");                          // 发送回车和换行
    Delay_ms(waitTime);                                // 等待模块响应
    if (reply1 == NULL && reply2 == NULL) return true; // 不需要回复
    len = Serial_USART_ReceiveData(buffer);
    // 判定回复结果
    if (len > 0) {
        if (reply1 && strstr((char *)buffer, reply1)) return true;
        if (reply2 && strstr((char *)buffer, reply2)) return true;
    } 
    return false; // 未收到期望回复
}

// 开启自动DHCP
bool ESP8266_DHCP_CUR()
{
    char cCmd[40];

    sprintf(cCmd, "AT+CWDHCP_CUR=1,1");

    return ESP8266_SendCommand(cCmd, "OK", NULL, 500);
}

// TCP 连接封装
bool ESP8266_ConnectTCP(char *ssid, char *password, char *serverAddr, uint16_t serverPort)
{
    char cCmd[128];
    if (!ESP8266_SendCommand("AT", "OK", NULL, 500)) return false;           // 测试模块是否工作
    ESP8266_SendCommand("ATE0", "OK", NULL, 500);                            // 关闭回显
    ESP8266_SendCommand("AT+CWMODE=1", "OK", "NULL", 500);                   // 设置STA模式，即客户端模式
    sprintf(cCmd, "AT+CWJAP= \"%s\",\"%s\"", ssid, password);                // 连接WIFI
    if (!ESP8266_SendCommand(cCmd, "WIFI GOT IP", NULL, 5000)) return false; // 等待连接成功
    ESP8266_DHCP_CUR();                                                      // 启用DHCP
    ESP8266_SendCommand("AT+CIFSR", "OK", NULL, 500);                        // 获取本机IP地址

    // 建立TCP连接
    sprintf(cCmd, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"", serverAddr, serverPort);
    return ESP8266_SendCommand(cCmd, "CONNECT", "ALREADY CONNECTED", 5000);
}

// 发送数据
bool ESP8266_SendData(char *data)
{
    char cCmd[128];
    sprintf(cCmd, "AT+CIPSEND=%d", strlen(data));
    if (!ESP8266_SendCommand(cCmd, ">", NULL, 2000)) return false;
    return ESP8266_SendCommand(data, "SEND OK", NULL, 2000);
}
