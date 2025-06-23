#include "stm32f10x.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "string.h"
#include "./drive/inc/EEPROM.h"

char msg[] = "Hello, EEPROM!"; // 要写入EEPROM的数据

char buffer[256]; // 用于接收数据的缓冲区

void EEPROM_IIC_Byte_Test(void){
    uint8_t i = 0;
    uint8_t s = 's'; // 测试数据
    EEPROM_IIC_WriteByte(0xA0, 0, s); // 向EEPROM写入一个字节
    EEPROM_IIC_ReadByte(0xA0, 0); // 从EEPROM读取一个字节
    OLED_ShowString(1, 1, buffer); // 在OLED上显示读取的数据
}

void EEPROM_IIC_Bytes_Test(void)
{
    EEPROM_IIC_WriteBytes(0xA0, 0, msg, strlen(msg)); // 初始化EEPROM，写入数据
    EEPROM_IIC_ReadBytes(0xA0, 0, buffer, sizeof(buffer)); // 从EEPROM读取数据到缓冲区
    OLED_ShowString(1, 1, buffer); // 在OLED上显示初始化数据
}

int main(void)
{
    EEPROM_IIC_Init();  // 初始化I2C
    OLED_Init(); // 初始化OLED
    while (1) {
        EEPROM_IIC_Byte_Test(); // 测试EEPROM读写
}

