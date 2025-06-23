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
#include "./drive/inc/MyDelay.h"

#define EEPROM_IIC_Write_SCL GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)())
#define EEPROM_IIC_Write_SDA GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)())
/* STM32自身的I2C地址，这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 0X0A
/* STM32 I2C 速率 */
#define I2C_Speed 400000

/*通讯等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


/****************************************  初始化 *******************************************/
void EEPROM_IIC_GPIO_Init(void)
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

void EEPROM_IIC_Mode_Config(void)
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

void EEPROM_IIC_Init(void)
{
    EEPROM_IIC_GPIO_Init();   // Initialize GPIO for I2C
    EEPROM_IIC_Mode_Config(); // Configure I2C mode
}

/****************************************  功能实现 *******************************************/
/**
 * @brief   等待I2C事件完成
 * @param   I2Cx: I2C外设
 * @param   event: 要等待的事件
 * @param   timeout: 超时时间
 * @retval  1: 成功，0: 超时
 */
uint8_t EEPROM_IIC_WaitEvent(I2C_TypeDef *I2Cx, uint32_t event)
{
    uint16_t TimeOut = I2CT_FLAG_TIMEOUT; // 设置超时时间
    while (!I2C_CheckEvent(I2Cx, event)) {
        if (TimeOut-- == 0) {
            return 0; // 超时
        }
    }
    return 1; // 成功
}

/**
 * @brief   产生I2C启动信号
 * @param   I2Cx: I2C外设
 * @retval  无
 */
void EEPROM_IIC_Start(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
}

/**
 * @brief   产生I2C停止信号
 * @param   I2Cx: I2C外设
 * @retval  无
 */
void EEPROM_IIC_Stop(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTOP(I2Cx, ENABLE);
}

/**
 * @brief  发送I2C地址
 * @param  I2Cx: I2C外设
 * @param  addr: 设备地址
 * @param  direction: 传输方向（I2C_Direction_Transmitter 或 I2C_Direction_Receiver）
 * @note   发送地址后会等待相应的事件，确保设备已准备好接收或发送数据。
 * @retval Null
 * */
void EEPROM_IIC_SendAddr(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t direction)
{
    I2C_Send7bitAddress(I2Cx, addr, direction);
    if (direction == I2C_Direction_Transmitter)
        while (EEPROM_IIC_WaitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    else
        while (EEPROM_IIC_WaitEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
}

/**
 * @brief  发送I2C数据
 * @param  I2Cx: I2C外设
 * @param  data: 要发送的数据
 * @note   发送数据后会等待相应的事件，确保数据已被发送。
 * @retval Null
 * */
void EEPROM_IIC_SendData(I2C_TypeDef *I2Cx, uint8_t data)
{
    I2C_SendData(I2Cx, data);
    while (EEPROM_IIC_WaitEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/**
 * @brief  接收I2C数据并发送ACK或NACK
 * @param  I2Cx: I2C外设
 * @param  data: 接收到的数据指针
 * @param  ack: 是否发送ACK（1: ACK, 0: NACK）
 * @retval 无
 * @note   接收数据后会等待相应的事件，确保数据已被接收。
 */
uint8_t EEPROM_IIC_ReceiveData(I2C_TypeDef *I2Cx, uint8_t ack)
{
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
    if (ack) {
        I2C_AcknowledgeConfig(I2Cx, ENABLE); // 发送ACK
    } else {
        I2C_AcknowledgeConfig(I2Cx, DISABLE); // 发送NACK
    }
    return I2C_ReceiveData(I2Cx); // 返回接收到的数据
}

/**
* @brief  向EEPROM写入一个字节
* @param  deviceAddr: 设备地址
* @param  memAddr: 内存地址
* @param  data: 要写入的数据
* @retval Null
* */
void EEPROM_IIC_WriteByte(uint8_t deviceAddr, uint16_t memAddr, uint8_t data)
{
    // 等待I2C1空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));

    // 发送起始信号
    EEPROM_IIC_Start(I2C1);
    // 发送设备地址和写操作
    EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Transmitter);
    // 发送内存地址（高字节）
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr >> 8));
    // 发送内存地址（低字节）
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr & 0xFF));
    // 发送数据
    EEPROM_IIC_SendData(I2C1, data);
    // 产生停止信号
    EEPROM_IIC_Stop(I2C1);
    Delay_ms(5); // 等待写入完成，EEPROM需要时间来完成写操作
}

/**
 * @brief  写入多个字节到EEPROM
 * @param  deviceAddr: 设备地址
 * @param  memAddr: 内存地址
 * @param  buffer: 数据缓冲区指针
 * @param  length: 要写入的字节数
 * @retval Null
 * */
void EEPROM_IIC_WriteBytes(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint8_t length)
{
    // TODO: 可以直接for循环调用EEPROM_IIC_Writebytes函数来实现写入多个字节
    // 等待I2C1空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));

    EEPROM_IIC_Start(I2C1);                                           // 产生起始信号
    EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Transmitter); // 发送设备地址
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr >> 8));               // 发送内存地址之高八位地址
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr && 0xFF));            // 发送内存地址之低八位地址

    // 发送数据，需要处理最后一个字节的ACK/NACK
    for (uint8_t i = 0; i < length; i++) {
        if (i == length - 1) {
            buffer[i] = EEPROM_IIC_ReceiveData(I2C1, 0); // NACK
        } else {
            buffer[i] = EEPROM_IIC_ReceiveData(I2C1, 1); // ACK
        }
    }
    EEPROM_IIC_Stop(I2C1); // 产生停止信号
}

/**
 * @brief  写入多个页到EEPROM
 * @param  deviceAddr: 设备地址
 * @param  memAddr: 起始内存地址
 * @param  buffer: 数据缓冲区指针
 * @param  length: 要写入的字节数
 * @param  pageSize: 每页的字节数
 * @retval Null
 */
void EEPROM_IIC_WritePages(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint16_t length, uint8_t pageSize)
{
    uint16_t remaining = length;
    uint16_t offset = 0;

    while (remaining > 0) {
        uint8_t writeLength = (remaining > pageSize) ? pageSize : remaining;

        // 等待I2C1空闲
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
        while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));

        EEPROM_IIC_Start(I2C1);                                           // 产生起始信号
        EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Transmitter); // 发送设备地址
        EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr >> 8));               // 发送内存地址之高八位地址
        EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr & 0xFF));             // 发送内存地址之低八位地址

        // 发送数据
        for (uint8_t i = 0; i < writeLength; i++) {
            EEPROM_IIC_SendData(I2C1, buffer[offset + i]);
        }

        EEPROM_IIC_Stop(I2C1); // 产生停止信号
        Delay_ms(5);    // 等待写入完成，EEPROM需要时间来完成写操作

        remaining -= writeLength;
        offset += writeLength;
        memAddr += writeLength;
    }
}

/**
* @brief  从EEPROM读取一个字节
* @param  deviceAddr: 设备地址
* @param  memAddr: 内存地址
* @retval data: 读取到的数据
* */
uint8_t EEPROM_IIC_ReadByte(uint8_t deviceAddr, uint16_t memAddr)
{
    uint8_t data;
    // 等待I2C1空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));

    EEPROM_IIC_Start(I2C1);// 发送起始信号
    EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Transmitter);// 发送设备地址和写操作
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr >> 8));// 发送内存地址（高字节）   
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr & 0xFF));// 发送内存地址（低字节）  
    EEPROM_IIC_Start(I2C1);// 发送重复起始信号
    EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Receiver);// 发送设备地址和读操作
    
    // 接收数据
    data = EEPROM_IIC_ReceiveData(I2C1, 0); // NACK
    EEPROM_IIC_Stop(I2C1);// 产生停止信号
    return data; // 返回接收到的数据
}

/**
 * @brief  从EEPROM读取多个字节
 * @param  deviceAddr: 设备地址
 * @param  memAddr: 内存地址
 * @param  buffer: 数据缓冲区指针
 * @param  length: 要读取的字节数
 * @retval Null
 */
void EEPROM_IIC_ReadBytes(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint8_t length)
{
    // 等待I2C1空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));

    EEPROM_IIC_Start(I2C1); // 产生起始信号
    EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Transmitter); // 发送设备地址和写操作
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr >> 8)); // 发送内存地址（高字节）
    EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr & 0xFF)); // 发送内存地址（低字节）
    EEPROM_IIC_Start(I2C1); // 发送重复起始信号    
    EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Receiver); // 发送设备地址和读操作 
    // 接收数据
    for (uint8_t i = 0; i < length; i++) {  
        if (i == length - 1) {
            buffer[i] = EEPROM_IIC_ReceiveData(I2C1, 0); // NACK
        } else {
            buffer[i] = EEPROM_IIC_ReceiveData(I2C1, 1); // ACK
        }
    }
    EEPROM_IIC_Stop(I2C1); // 产生停止信号
}



/**
 * @brief  读取多个页从EEPROM
 * @param  deviceAddr: 设备地址
 * @param  memAddr: 起始内存地址
 * @param  buffer: 数据缓冲区指针
 * @param  length: 要读取的字节数
 * @param  pageSize: 每页的字节数
 * @retval Null
 */
void EEPROM_IIC_ReadPages(uint8_t deviceAddr, uint16_t memAddr, char *buffer, uint16_t length, uint8_t pageSize)
{
    uint16_t remaining = length;
    uint16_t offset = 0;

    while (remaining > 0) {
        uint8_t readLength = (remaining > pageSize) ? pageSize : remaining;

        // 等待I2C1空闲
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
        while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE));

        EEPROM_IIC_Start(I2C1);                                           // 产生起始信号
        EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Transmitter); // 发送设备地址和写操作
        EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr >> 8));               // 发送内存地址之高八位地址
        EEPROM_IIC_SendData(I2C1, (uint8_t)(memAddr & 0xFF));             // 发送内存地址之低八位地址
        EEPROM_IIC_Start(I2C1);                                           // 发送重复起始信号
        EEPROM_IIC_SendAddr(I2C1, deviceAddr, I2C_Direction_Receiver);    // 发送设备地址和读操作

        // 接收数据
        for (uint8_t i = 0; i < readLength; i++) {
            if (i == readLength - 1) {
                buffer[offset + i] = EEPROM_IIC_ReceiveData(I2C1, 0); // NACK
            } else {
                buffer[offset + i] = EEPROM_IIC_ReceiveData(I2C1, 1); // ACK
            }
        }

        EEPROM_IIC_Stop(I2C1); // 产生停止信号

        remaining -= readLength;
        offset += readLength;
        memAddr += readLength;
    }
}

