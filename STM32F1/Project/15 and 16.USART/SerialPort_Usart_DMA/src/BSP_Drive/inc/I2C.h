#ifndef _I2C_H
#define _I2C_H


void I2C_Init(void);
void I2C_SendData(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len);
void I2C_ReceiveData(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len);

#endif
