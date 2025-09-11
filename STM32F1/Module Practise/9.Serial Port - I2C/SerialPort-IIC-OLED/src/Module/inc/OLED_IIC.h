#ifndef __OLED_IIC_H__
#define __OLED_IIC_H__

#include "stm32f10x.h"

typedef enum {
    I2C_OK = 0, // 成功
    I2C_ERR_TIMEOUT, // 超时
    I2C_ERR_ACK_FAIL, // 应答失败
    I2C_ERR_PARAM, // 参数错误
    I2C_ERR_BUSY, // 总线忙
    I2C_ERR_UNKNOWN // 未知错误
} I2C_Status_t;

#endif // !__OLED_IIC_H__