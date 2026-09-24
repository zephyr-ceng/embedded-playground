/****************
 *@description: EEPROM IIC Serial Port Driver
 *@author: zephyr
 *@date: 2025-09-30 16:17:42
 *@version: V1.0.0
****************/

#ifndef __SERIALPORT_IIC_H
#define __SERIALPORT_IIC_H

#include "stm32f10x.h"
#include "./Module/inc/MyDelay.h"
#include "stddef.h"

typedef enum {
    I2C_ERROR_NONE = 0,           // No error
    I2C_ERROR_START_TIMEOUT,      // 起始信号超时
    I2C_ERROR_ADDR_TIMEOUT,       // 发送设备地址超时
    I2C_ERROR_DATA_TIMEOUT,       // 发送数据超时
    I2C_ERROR_WRONG_DIRECTION,    // 错误的读写方向
    I2C_ERROR_MAX_RETRIES,        // 超出最大重试次数
    I2C_ERROR_UNKNOWN             // Unknown error
} I2C_ErrorCode_t;

typedef void  (*I2C_TimeoutCallback_t)(I2C_ErrorCode_t errorcode); // 定义函数指针类型
uint8_t EEPROM_WriteByte(uint8_t addr, uint16_t MemoryAddr, uint8_t data);
uint8_t EEPROM_WriteBytes(uint8_t addr, uint16_t MemoryAddr, uint8_t *data, uint16_t len);
#endif // !__SERIALPORT_IIC_H