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
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
#endif // !__OLED_IIC_H__