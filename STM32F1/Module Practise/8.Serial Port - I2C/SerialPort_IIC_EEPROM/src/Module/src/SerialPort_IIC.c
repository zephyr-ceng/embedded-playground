#include "./Module/inc/SerialPort_IIC.h"

/* errorcode:
 * 0: 起始信号超时
 * 1: 发送设备地址超时
 * 2: 发送数据超时
 * 3: 错误的读写方向
 * 4: 超出最大重试次数
 */

#define I2C_Speed         400000 // 400kHz standard mode
#define I2C_OWN_ADDRESS7  0x00   // 本机地址
#define OLED_I2C_ADDR     0x78   // 八位地址
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
#define EEPROM_MAX_TRIES  300

static I2C_TimeoutCallback_t callback_cb = NULL; // .c 定义全局变量指针

void I2C_RegisterTimeoutCallback(I2C_TimeoutCallback_t cb)
{
    callback_cb = cb;
}
/********************************************** I2C初始化 **********************************************/
void I2C_InitConfiguration()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    // GPIO初始化
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD; // 设置GPIO模式为复用开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // I2C 初始化
    I2C_DeInit(I2C2);                                            // Reset I2C peripheral
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;    // I2C模式
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2; // 快速模式占空比2
    I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;               // 使能应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址
    I2C_InitStructure.I2C_ClockSpeed          = I2C_Speed;                    // 100kHz standard mode
    I2C_Init(I2C2, &I2C_InitStructure);

    I2C_Cmd(I2C2, ENABLE);
}

/********************************************** 基础函数封装 **********************************************/
// 超时等待
static uint32_t I2C_Timeout_userCallback(I2C_ErrorCode_t errorCode)
{
    if (callback_cb != NULL) {
        callback_cb(errorCode); // 调用注册的回调函数
    }
    return 0; // 超时
}

// 起始信号
I2C_ErrorCode_t IIC_Start()
{
    uint32_t TIMEOUT = I2CT_FLAG_TIMEOUT;
    I2C_GenerateSTART(I2C2, ENABLE); // 产生I2C起始信号
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
        if (TIMEOUT-- == 0) return I2C_Timeout_userCallback(I2C_ERROR_START_TIMEOUT); // 起始信号超时
    return 1;
}

// 地址发送
I2C_ErrorCode_t IIC_SendADDR(uint8_t address, uint32_t direction)
{
    uint32_t TIMEOUT = I2CT_FLAG_TIMEOUT;
    if ((address & 0xFE) != address) address >>= 1; // 确保地址为七位
    if (direction != I2C_Direction_Transmitter && direction != I2C_Direction_Receiver)
        return I2C_Timeout_userCallback(I2C_ERROR_WRONG_DIRECTION); // 错误的读写方向

    I2C_Send7bitAddress(I2C2, address, direction); // 发送器件地址+信号
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        if (TIMEOUT-- == 0) return I2C_Timeout_userCallback(I2C_ERROR_ADDR_TIMEOUT); // 发送设备地址超时
    return 1;
}

// 发送数据
I2C_ErrorCode_t IIC_SendDataByte(uint8_t data)
{
    uint32_t TIMEOUT = I2CT_FLAG_TIMEOUT;
    I2C_SendData(I2C2, data); // 发送数据
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        if (TIMEOUT-- == 0) return I2C_Timeout_userCallback(I2C_ERROR_DATA_TIMEOUT); // 发送数据超时
    return 1;
}

// 停止信号
void IIC_Stop()
{
    I2C_GenerateSTOP(I2C2, ENABLE); // 产生I2C停止信号
}
/********************************************** EEPROM函数封装 **********************************************/

uint8_t EEPROM_WaitStatus()
{
    // 检测EEPROM是否忙碌以及超时等待
    uint32_t TIMEOUT = I2CT_LONG_TIMEOUT;
    uint16_t tries   = EEPROM_MAX_TRIES;
    uint32_t temp;
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
        if (TIMEOUT-- == 0) return I2C_Timeout_userCallback(0); // 超时
    while (tries--) {
        temp = IIC_Start(); // 发送起始信号
        if (temp == 0) return temp;
        if (IIC_SendADDR(OLED_I2C_ADDR, I2C_Direction_Transmitter)) {
            IIC_Stop(); // 停止信号
            return 1;   // 从机应答成功
        }
    }
    return I2C_Timeout_userCallback(I2C_ERROR_MAX_RETRIES); // 超出最大重试次数
}

// 向EEPROM写入一个字节
uint8_t EEPROM_WriteByte(uint8_t addr, uint16_t MemoryAddr, uint8_t data)
{
    uint32_t temp;
    temp = IIC_Start(); // 发送起始信号
    if (temp == 0) return temp;
    temp = IIC_SendADDR(OLED_I2C_ADDR, I2C_Direction_Transmitter); // 发送器件地址+写信号
    if (temp == 0) return temp;
    temp = IIC_SendDataByte(addr); // 发送数据地址
    if (temp == 0) return temp;
    temp = IIC_SendDataByte((uint8_t)MemoryAddr >> 8); // 发送EEPROM高地址
    if (temp == 0) return temp;
    temp = IIC_SendDataByte((uint8_t)MemoryAddr && 0xFF); // 发送EEPROM低地址
    if (temp == 0) return temp;
    temp = IIC_SendDataByte(data); // 发送数据
    if (temp == 0) return temp;
    IIC_Stop();  // 发送停止信号
    Delay_ms(5); // 等待写入完成
    return 1;
}

// 向EEPROM写入多个字节
uint8_t EEPROM_WriteBytes(uint8_t addr, uint16_t MemoryAddr, uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint8_t res = 0;
    for (i = 0; i < len; i++) {
        // 等待上次写入完成, 主要检测EEPROM处是否应答，应答会将SCL拉低)
        if (EEPROM_WaitStatus())
            res = EEPROM_WriteByte(addr, MemoryAddr++, data[i]);
    }
    return res;
}
