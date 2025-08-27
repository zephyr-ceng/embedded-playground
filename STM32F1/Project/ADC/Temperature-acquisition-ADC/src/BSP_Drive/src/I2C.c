#include "./BSP_Drive/inc/I2C.h"
// #include "stm32f10x_i2c.h"

/* STM32自身的I2C地址，这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 0x00
// STM32 I2C 速率 
#define I2C_Speed 400000
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

void I2C_GPIO_Init(I2C_TypeDef *I2Cx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    if (I2Cx == I2C1) {
        // Enable I2C1 clock
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

        // Configure I2C1 SCL and SDA pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7; // SCL (PB6), SDA (PB7)
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    } else if (I2Cx == I2C2) {
        // Enable I2C2 clock
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

        // Configure I2C2 SCL and SDA pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11; // SCL (PB10), SDA (PB11)
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

/**
 * @brief  I2C模式配置, 占空比、设备地址、ack应答使能、通信速率等（通信速率和占空比需要匹配）
 * @param  Null
 * @retval Null
 * */
void I2C_Mode_Config(I2C_TypeDef *I2Cx)
{
    I2C_InitTypeDef I2C_InitStructure;
    I2C_DeInit(I2Cx); // Reset I2C peripheral
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = I2C_Speed; // 100kHz standard mode
    I2C_Init(I2Cx, &I2C_InitStructure);
    // Enable I2C1 peripheral clock
    I2C_Cmd(I2Cx, ENABLE); // Enable I2C1
}

// I2C初始化函数
void I2C_Config_Init(I2C_TypeDef *I2Cx)
{
    I2C_GPIO_Init(I2Cx); // Initialize GPIO for I2C
    I2C_Mode_Config(I2Cx); // Configure I2C mode
}

/****************************************  I2C基本功能实现 *******************************************/
// I2C 传输超时判定（0：timeout 1 success）
uint8_t I2C_waitEvent(I2C_TypeDef *I2Cx, uint32_t event)
{
    uint32_t timeout = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, event)) {
        if (timeout-- == 0) {
            return 0; // Timeout
        }
    }
    return 1;
}

void I2C_Start(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTART(I2Cx, ENABLE);                             // 相当于设置scl高电平，sda由高电平转低电平
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); // 阻塞等待，检查I2C状态机
}

// 从设备发送停止信号
void I2C_Slave_Stop(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTOP(I2Cx, ENABLE);                  // 相当于scl高电平，sda线从低电平转为高电平
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF)); // 检查停止标志位，主设备不需要，从设备需要
}

// 主设备发送停止信号
void I2C_Master_Stop(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTOP(I2Cx, ENABLE);                 // 发送停止信号
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)); // 等待I2C总线空闲
}

/**
 * @brief  发送I2C地址
 * @param  I2Cx: 指向I2C外设的指针
 * @param  addr: 7位I2C地址
 * @param  direction: I2C方向（发送-I2C_Direction_Transmitter 或 接收-I2C_Direction_Receiver）
 * @retval Null
 * */
void I2C_SendAddr(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t direction)
{
    I2C_Send7bitAddress(I2Cx, addr, direction);
    if (direction == I2C_Direction_Transmitter)
        while (I2C_waitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    else
        while (I2C_waitEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
}


void I2C_SendByte(I2C_TypeDef *I2Cx, uint8_t data)
{
    I2C_SendData(I2Cx, data);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/**
 * @brief  发送数据到I2C设备
 * @param  I2Cx: 指向I2C外设的指针
 * @param  addr: I2C设备地址
 * @param  data: 指向要发送的数据的指针
 * @param  len: 要发送的数据长度
 * @retval Null
 * */
void I2C_Send_Data(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len)
{
    I2C_Start(I2Cx);                                     // 发送起始信号
    I2C_SendAddr(I2Cx, addr, I2C_Direction_Transmitter); // 发送设备地址和写方向
    for (uint16_t i = 0; i < len; i++) {
        I2C_SendByte(I2Cx, data[i]); // 发送数据字节
    }
    I2C_Master_Stop(I2Cx); // 发送停止信号（发送方为主机）
}

/**
 * @brief  从I2C设备接收数据
 * @param  I2Cx: 指向I2C外设的指针
 * @param  addr: I2C设备地址
 * @param  data: 指向接收数据的缓冲区指针
 * @param  len: 要接收的数据长度
 * @retval Null
 * */
void I2C_Receive_Data(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len)
{
    I2C_Start(I2Cx);                                  // 发送起始信号
    I2C_SendAddr(I2Cx, addr, I2C_Direction_Receiver); // 发送设备地址和读方向
    for (uint16_t i = 0; i < len; i++) {
        if (i == len - 1) {
            I2C_AcknowledgeConfig(I2Cx, DISABLE); // 最后一个字节不需要应答
        }
        data[i] = I2C_ReceiveData(I2Cx); // 接收数据字节（接收方为从机）
    }
    I2C_Slave_Stop(I2Cx); // 发送停止信号
}