#ifndef _I2C_H
#define _I2C_H
#include "stm32f10x.h"

typedef struct
{
    I2C_TypeDef *I2Cx;
    uint32_t I2C_ClockSpeed; /*!< Specifies the clock frequency.
                                This parameter must be set to a value lower than 400kHz */

    uint16_t I2C_Mode; /*!< Specifies the I2C mode.
                          This parameter can be a value of @ref I2C_mode */

    uint16_t I2C_DutyCycle; /*!< Specifies the I2C fast mode duty cycle.
                                 This parameter can be a value of @ref I2C_duty_cycle_in_fast_mode */

    uint16_t I2C_OwnAddress1; /*!< Specifies the first device own address.
                                   This parameter can be a 7-bit or 10-bit address. */

    uint16_t I2C_Ack; /*!< Enables or disables the acknowledgement.
                           This parameter can be a value of @ref I2C_acknowledgement */

    uint16_t I2C_AcknowledgedAddress; /*!< Specifies if 7-bit or 10-bit address is acknowledged.
                                           This parameter can be a value of @ref I2C_acknowledged_address */
} I2C_Config_t;

void I2C_Config_Init(I2C_TypeDef *I2Cx);
void I2C_Send_Data(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len);
void I2C_Receive_Data(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len);

#endif

void I2C_InitModule(I2C_Config_t config)
{
    I2C_InitTypeDef I2C_InitStructure;

    if (I2Cx == I2C1)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    else if (I2Cx == I2C2)

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    I2C_DeInit(config.I2Cx);
    I2C_InitStructure.I2C_ClockSpeed = config.I2C_ClockSpeed;
    I2C_InitStructure.I2C_Mode = config.I2C_Mode;
    I2C_InitStructure.I2C_DutyCycle = config.I2C_DutyCycle;
    I2C_InitStructure.I2C_OwnAddress1 = config.I2C_OwnAddress1;
    I2C_InitStructure.I2C_Ack = config.I2C_Ack;
    I2C_InitStructure.I2C_AcknowledgedAddress = config.I2C_AcknowledgedAddress;
}