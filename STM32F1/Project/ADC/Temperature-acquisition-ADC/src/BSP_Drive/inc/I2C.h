#ifndef _I2C_H
#define _I2C_H


#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stddef.h"

#define I2C_Speed 400000 // 100kHz standard mode
#define I2C_OWN_ADDRESS7 0x00 // 本机地址
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000) // Timeout 1s
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT)) // Timeout 10s

// I2C状态枚举
typedef enum {
    I2C_OK = 0, // 成功
    I2C_ERR_TIMEOUT, // 超时
    I2C_ERR_ACK_FAIL, // 应答失败
    I2C_ERR_PARAM, // 参数错误
    I2C_ERR_BUSY, // 总线忙
    I2C_ERR_UNKNOWN // 未知错误
} I2C_Status_t;

I2C_Status_t I2C_InitModule(I2C_TypeDef *I2Cx);
void I2C_Cmd_Set(I2C_TypeDef *I2Cx, FunctionalState NewState);
I2C_Status_t I2C_Master_TransmitData(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len, uint32_t timeout);
I2C_Status_t I2C_Master_TransmitData_WithRetry(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len, uint32_t timeout, uint8_t retry);
I2C_Status_t I2C_Master_ReceivesData(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len, uint32_t timeout);
I2C_Status_t I2C_Slave_ReceiveData(I2C_TypeDef *I2Cx, uint8_t *data, uint16_t len, uint32_t timeout);
I2C_Status_t I2C_Slave_TransmitData(I2C_TypeDef *I2Cx, uint8_t *data, uint16_t len, uint32_t timeout);  
#endif