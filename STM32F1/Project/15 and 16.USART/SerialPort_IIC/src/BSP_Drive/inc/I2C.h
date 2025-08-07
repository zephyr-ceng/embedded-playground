#ifndef _I2C_H
#define _I2C_H
#include "stm32f10x.h"

void I2C_Config_Init(I2C_TypeDef *I2Cx);
void I2C_Send_Data(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len);
void I2C_Receive_Data(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len);

#endif
