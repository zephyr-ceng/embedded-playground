#include "./BSP_Drive/inc/I2C.h"

/**
 * @brief  I2C 外设使能控制
 * @param  I2Cx: 指向I2C外设的指针
 * @param  NewState: ENABLE or DISABLE
 * @retval Null
 * */
I2C_Status_t I2C_InitModule(I2C_TypeDef *I2Cx)
{
    I2C_InitTypeDef I2C_InitStructure;
    if (I2Cx == I2C1)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    else if (I2Cx == I2C2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    else
        return I2C_ERR_PARAM; // 参数错误

    I2C_DeInit(I2Cx);                                            // Reset I2C peripheral
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;    // I2C模式
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2; // 快速模式占空比2
    I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;               // 使能应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址
    I2C_InitStructure.I2C_ClockSpeed          = I2C_Speed;                    // 100kHz standard mode
    I2C_Init(I2Cx, &I2C_InitStructure);
    I2C_Cmd_Set(I2Cx, ENABLE); // Enable I2C1
    return I2C_OK;
}

// I2C 外设使能控制
void I2C_Set_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if (I2Cx == NULL) return I2C_ERR_PARAM; // 参数错误
    if (NewState != DISABLE) {
        I2C_Cmd(I2Cx, ENABLE);
    } else {
        I2C_Cmd(I2Cx, DISABLE);
    }
}

/****************************************  I2C基础封装 *******************************************/
/**
 * @brief  等待I2C事件，带超时处理
 * @param  I2Cx: 指向I2C外设的指针
 * @param  event: 要等待的I2C事件
 * @param  timeout: 超时时间
 * @retval I2C状态（I2C_OK, I2C_ERR_TIMEOUT, I2C_ERR_ACK_FAIL, I2C_ERR_UNKNOWN, I2C_ERR_PARAM）
 * */
I2C_Status_t I2C_WaitEventUntilTimeout(I2C_TypeDef *I2Cx, uint32_t event, uint32_t timeout)
{
    if (I2Cx == NULL) return I2C_ERR_PARAM; // 参数错误
    while (!I2C_CheckEvent(I2Cx, event)) {
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
            /* 清 AF 并返回 ACK 失败 */
            I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
            return I2C_ERR_ACK_FAIL;
        }
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BERR) == SET) {
            I2C_ClearFlag(I2Cx, I2C_FLAG_BERR);
            return I2C_ERR_UNKNOWN;
        }
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ARLO) == SET) {
            I2C_ClearFlag(I2Cx, I2C_FLAG_ARLO);
            return I2C_ERR_UNKNOWN;
        }
        if (timeout-- == 0) return I2C_ERR_TIMEOUT; // Timeout
    }
    return I2C_OK;
}

// 主设备发送起始信号
I2C_Status_t I2C_Start(I2C_TypeDef *I2Cx, uint32_t timeout)
{
    uint32_t t = timeout;
    if (I2Cx == NULL) return I2C_ERR_PARAM;                                   // 参数错误
    if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) != RESET) return I2C_ERR_BUSY; // 总线忙
    I2C_GenerateSTART(I2Cx, ENABLE);                                          // 相当于设置scl高电平，sda由高电平转低电平
    return I2C_WaitEventUntilTimeout(I2Cx, I2C_EVENT_MASTER_MODE_SELECT, t);  // 等待EV5事件
}

// 主设备发送停止信号
I2C_Status_t I2C_Master_Stop(I2C_TypeDef *I2Cx, uint32_t timeout)
{
    uint32_t t = timeout;
    if (I2Cx == NULL) return I2C_ERR_PARAM; // 参数错误
    I2C_GenerateSTOP(I2Cx, ENABLE);         // 发送停止信号

    // 等待I2C总线空闲
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) != RESET) {
        if (t-- == 0) return I2C_ERR_TIMEOUT; // Timeout
    }
    return I2C_OK;
}

// 从设备发送停止信号,需要重新使能I2C外设，这样从机就可以持续接收数据， 可以在中断中使用
I2C_Status_t I2C_Slave_Stop(I2C_TypeDef *I2Cx, uint32_t timeout)
{
    if (I2Cx == NULL) return I2C_ERR_PARAM; // 参数错误
    // 需要检测StopF标志位，确保停止信号发送完成
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET) {
        // 清除停止标志
        if (timeout-- == 0) {
            return I2C_ERR_TIMEOUT; // Timeout
        }
    }
    (void)I2Cx->SR1;            // 读SR1清除STOPF标志, SR2没有STOPF标志，主要用于辅助状态，如BUSY
    I2C_Set_Cmd(I2Cx, DISABLE); // 先关闭I2C外设
    I2C_Set_Cmd(I2Cx, ENABLE);  // 重新使能I2C外设
    return I2C_OK;
}

/**
 * @brief  发送I2C地址
 * @param  I2Cx: 指向I2C外设的指针
 * @param  addr: 7位I2C地址
 * @param  direction: I2C方向（I2C_Direction_Transmitter-发送 或 I2C_Direction_Receiver-接收）
 * @retval Null
 * */
I2C_Status_t I2C_SendAddr(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t direction, uint32_t timeout)
{
    if (I2Cx == NULL) return I2C_ERR_PARAM;
    if (addr <= 0x7F) {                                                                                          // 参数错误
        if (direction != I2C_Direction_Transmitter && direction != I2C_Direction_Receiver) return I2C_ERR_PARAM; // 参数错误
        I2C_Send7bitAddress(I2Cx, addr, direction);

        uint32_t t = timeout;
        // 检查是否进入EV6状态
        while (t--) {
            if (direction == I2C_Direction_Transmitter) {
                if (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) return I2C_OK; // 发送模式
            } else {
                if (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) return I2C_OK; // 接收模式
            }

            // 检查ACK失败标志AF
            if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) != RESET) {
                I2C_ClearFlag(I2Cx, I2C_FLAG_AF); // 清除ACK失败标志
                I2C_GenerateSTOP(I2Cx, ENABLE);   // 发送停止信号
                return I2C_ERR_ACK_FAIL;          // 应答失败
            }

            // 其他错误
            if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BERR) != RESET) {
                I2C_ClearFlag(I2Cx, I2C_FLAG_BERR); // 清除总线错误标志
                I2C_GenerateSTOP(I2Cx, ENABLE);     // 发送停止信号
                return I2C_ERR_UNKNOWN;             // 未知错误
            }

            // ARLO 错误
            if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ARLO) != RESET) {
                I2C_ClearFlag(I2Cx, I2C_FLAG_ARLO); // 清除仲裁丢失标志
                I2C_GenerateSTOP(I2Cx, ENABLE);     // 发送停止信号
                return I2C_ERR_UNKNOWN;             // 未知错误
            }
        }
        I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
        return I2C_ERR_TIMEOUT;         // Timeout
    } else {
        return I2C_ERR_PARAM; // 参数错误
    }
}

/**
 * @brief  发送一个字节数据到I2C设备
 * @param  I2Cx: 指向I2C外设的指针
 * @param  data: 要发送的数据
 * @retval Null
 * */
I2C_Status_t I2C_SendByte(I2C_TypeDef *I2Cx, uint8_t data, uint32_t timeout)
{
    I2C_SendData(I2Cx, data);
    return I2C_WaitEventUntilTimeout(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED, timeout); // 等待数据发送完成
}

/****************************************  I2C功能封装 *******************************************/
/**
 * @brief  无重试机制发送数据到I2C设备
 * @param  I2Cx: 指向I2C外设的指针
 * @param  addr: I2C设备地址
 * @param  data: 指向要发送的数据的指针
 * @param  len: 要发送的数据长度
 * @retval I2C状态（I2C_OK, I2C_ERR_TIMEOUT, I2C_ERR_ACK_FAIL, I2C_ERR_BUSY, I2C_ERR_UNKNOWN, I2C_ERR_PARAM）
 * */
I2C_Status_t I2C_Master_TransmitData(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len, uint32_t timeout)
{
    I2C_Status_t status;
    uint32_t t = timeout;
    if (I2Cx == NULL || data == NULL || len == 0) return I2C_ERR_PARAM;       // 参数错误
    if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) != RESET) return I2C_ERR_BUSY; // 总线忙

    // 发送起始信号
    status = I2C_Start(I2Cx, timeout);
    if (status != I2C_OK) return status;

    // 发送设备地址和写方向
    status = I2C_SendAddr(I2Cx, addr, I2C_Direction_Transmitter, timeout);
    if (status != I2C_OK) {
        I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
        return status;
    }
    // 发送数据
    for (uint16_t i = 0; i < len; i++) {
        status = I2C_SendByte(I2Cx, data[i], timeout);
        if (status != I2C_OK) {
            I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
            return status;
        }
    }

    // 发送停止信号
    return I2C_Master_Stop(I2Cx, timeout);
}

// 带重试机制的发送数据函数
I2C_Status_t I2C_Master_TransmitData_WithRetry(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len, uint32_t timeout, uint8_t retry)
{
    I2C_Status_t status;
    do {
        status = I2C_Send_Data(I2Cx, addr, data, len, timeout);
        if (status == I2C_OK) break; // 成功则退出
        retry--;
    } while (retry > 0);
    return status; // 返回最后一次的状态
}

/**
 * @brief  主机I2C设备接收数据
 * @param  I2Cx: 指向I2C外设的指针
 * @param  addr: I2C设备地址
 * @param  data: 指向接收数据的缓冲区指针
 * @param  len: 要接收的数据长度
 * @param  timeout: 超时时间
 * @retval I2C状态（I2C_OK, I2C_ERR_TIMEOUT, I2C_ERR_ACK_FAIL, I2C_ERR_BUSY, I2C_ERR_UNKNOWN, I2C_ERR_PARAM）
 * */
I2C_Status_t I2C_Master_ReceivesData(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t *data, uint16_t len, uint32_t timeout)
{
    I2C_Status_t status;
    // uint32_t t = timeout;
    if (I2Cx == NULL || data == NULL || len == 0) return I2C_ERR_PARAM;       // 参数错误
    if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) != RESET) return I2C_ERR_BUSY; // 总线忙

    status = I2C_Start(I2Cx, timeout); // 发送起始信号
    if (status != I2C_OK) return status;

    status = I2C_SendAddr(I2Cx, addr, I2C_Direction_Receiver, timeout); // 发送设备地址和读方向
    if (status != I2C_OK) {
        I2C_GenerateSTOP(I2Cx, ENABLE); // 发送停止信号
        return status;
    }
    for (uint16_t i = 0; i < len; i++) {

        if (i == len - 1) {
            I2C_AcknowledgeConfig(I2Cx, DISABLE); // 最后一个字节不需要应答
            I2C_GenerateSTOP(I2Cx, ENABLE);       // 发送停止信号,防止时序问题
        } else {
            I2C_AcknowledgeConfig(I2Cx, ENABLE); // 其他字节需要应答
        }
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) // 等待数据接收完成
        {
            if (timeout-- == 0) {
                I2C_AcknowledgeConfig(I2Cx, ENABLE); // 恢复应答功能
                I2C_GenerateSTOP(I2Cx, ENABLE);      // 发送停止信号
                return I2C_ERR_TIMEOUT;              // Timeout
            }
        }

        data[i] = I2C_ReceiveData(I2Cx); // 接收数据字节
    }
    I2C_AcknowledgeConfig(I2Cx, ENABLE);     // 恢复应答功能
    status = I2C_Master_Stop(I2Cx, timeout); // 发送停止信号
    if (status != I2C_OK) return status;
    return I2C_OK;
}

/**
 * @brief  从机I2C设备发送数据(阻塞模式)
 * @param  I2Cx: 指向I2C外设的指针
 * @param  data: 指向要发送的数据的指针
 * @param  len: 要发送的数据长度
 * @param  timeout: 超时时间
 * @retval I2C状态（I2C_OK, I2C_ERR_TIMEOUT, I2C_ERR_PARAM）
 * */
I2C_Status_t I2C_Slave_TransmitData(I2C_TypeDef *I2Cx, uint8_t *data, uint16_t len, uint32_t timeout)
{
    uint32_t t = timeout;
    if (I2Cx == NULL || data == NULL || len == 0) return I2C_ERR_PARAM; // 参数错误
    for (uint16_t i = 0; i < len; i++) {
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == RESET) // 等待数据发送完成
        {
            if (t-- == 0) {
                return I2C_ERR_TIMEOUT; // Timeout
            }
        }
        I2C_SendData(I2Cx, data[i]); // 发送数据字节（发送方为从机）
    }
    return I2C_OK;
}

/**
 * @brief  从机I2C设备接收数据
 * @param  I2Cx: 指向I2C外设的指针
 * @param  data: 指向接收数据的缓冲区指针
 * @param  len: 要接收的数据长度
 * @param  timeout: 超时时间
 * @retval I2C状态（I2C_OK, I2C_ERR_TIMEOUT, I2C_ERR_PARAM）
 * */
I2C_Status_t I2C_Slave_ReceiveData(I2C_TypeDef *I2Cx, uint8_t *data, uint16_t len, uint32_t timeout)
{
    if (I2Cx == NULL || data == NULL || len == 0) return I2C_ERR_PARAM; // 参数错误
    for (uint16_t i = 0; i < len; i++) {
        while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) // 等待数据接收完成
        {
            if (timeout-- == 0) {
                return I2C_ERR_TIMEOUT; // Timeout
            }
        }
        data[i] = I2C_ReceiveData(I2Cx); // 接收数据字节（接收方为从机）
    }
    return I2C_OK;
}
