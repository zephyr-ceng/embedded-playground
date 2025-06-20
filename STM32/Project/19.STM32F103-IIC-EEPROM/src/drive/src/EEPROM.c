/****************
 *@description: EEPROM driver implementation
 *@file: EEPROM.c
 *@author: zephyr
 *@date: 2025-06-20 17:54:56
 *@version: V1.0.0
 ****************/

#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_i2c.h"

#define IIC_Write_SCL GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)())
#define IIC_Write_SDA GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)())
/* STM32自身的I2C地址，这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 0X0A
/* STM32 I2C 速率 */
#define I2C_Speed 400000

/*通讯等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

void IIC_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // Configure SCL (PB6) and SDA (PB7) as open-drain outputs
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IIC_Mode_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure;
    I2C_DeInit(I2C1); // Reset I2C1
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = I2C_Speed; // 100kHz standard mode
    I2C_Init(I2C1, &I2C_InitStructure);
    // Enable I2C1 peripheral clock
    I2C_Cmd(I2C1, ENABLE); // Enable I2C1
}

void IIC_Init(void)
{
    IIC_GPIO_Init();   // Initialize GPIO for I2C
    IIC_Mode_Config(); // Configure I2C mode
}

/**
 * @brief   在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
 *          不能超过EEPROM页的大小，AT24C02每页有8个字节
 * @param   deviceAddr: 设备地址
 * @param   memAddr: 写入的内存地址
 * @param   pBuffer: 缓冲区指针
 * @param   NumByteToWrite: 要写的字节数，要求NumByteToWrite小于页大小
 * @retval  正常返回1，异常返回0
 */
/**
 * @brief   在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
 *          不能超过EEPROM页的大小，AT24C02每页有8个字节
 * @param   deviceAddr: 设备地址
 * @param   memAddr: 写入的内存地址
 * @param   pBuffer: 缓冲区指针
 * @param   NumByteToWrite: 要写的字节数，要求NumByteToWrite小于页大小
 * @retval  正常返回1，异常返回0
 */
uint8_t EEPROM_WritePage(uint8_t deviceAddr, uint16_t memAddr, uint8_t *pBuffer, uint8_t NumByteToWrite)
{
    // 等待I2C1空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    // 起始信号
    I2C_GenerateSTART(I2C1, ENABLE);
    uint16_t I2CTIMEOUT = I2CT_FLAG_TIMEOUT;

    // 等待EV5（START条件已发送）
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)){
        // 检查超时
        if (I2CTIMEOUT-- == 0) {
            I2C_GenerateSTOP(I2C1, ENABLE); // 产生STOP条件
            // TODO: 处理超时错误
            printf("I2C Timeout Error: EV5 not received\n");
        }
    };

    // 发送设备地址并指定写操作
    I2C_Send7bitAddress(I2C1, deviceAddr, I2C_Direction_Transmitter);

    // 等待EV6（地址已被应答）
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 发送内存地址（高字节，如果适用）
    I2C_SendData(I2C1, (uint8_t)(memAddr >> 8));

    // 等待EV8_2（数据字节已发送并被应答）
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 发送内存地址（低字节）
    I2C_SendData(I2C1, (uint8_t)(memAddr & 0xFF));

    // 等待EV8_2
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 写入数据字节
    for (uint8_t i = 0; i < NumByteToWrite; i++) {
        I2C_SendData(I2C1, pBuffer[i]);

        // 等待EV8_2
        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }

    // 产生STOP条件
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 1; // 正常返回
}
