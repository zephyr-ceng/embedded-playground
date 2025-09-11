#include "./Module/inc/OLED_IIC.h"

#define I2C_Speed        400000 // 100kHz standard mode
#define I2C_OWN_ADDRESS7 0x00   // 本机地址
#define OLED_I2C_ADDR 0x78

void I2C_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    // GPIO初始化
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD; // 设置GPIO模式为开漏输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // I2C 初始化
    I2C_DeInit(I2Cx);                                            // Reset I2C peripheral
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;    // I2C模式
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2; // 快速模式占空比2
    I2C_InitStructure.I2C_OwnAddress1         = I2C_OWN_ADDRESS7;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;               // 使能应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7位地址
    I2C_InitStructure.I2C_ClockSpeed          = I2C_Speed;                    // 100kHz standard mode
    I2C_Init(I2Cx, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}

// 超时时间判定
I2C_Status_t I2C_WaitEventUntilTimeout(uint32_t event, uint32_t timeout)
{
    while (!I2C_CheckEvent(I2C1, event)) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) {
            /* 清 AF 并返回 ACK 失败 */
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            return I2C_ERR_ACK_FAIL;
        }
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR) == SET) {
            I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
            return I2C_ERR_UNKNOWN;
        }
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_ARLO) == SET) {
            I2C_ClearFlag(I2C1, I2C_FLAG_ARLO);
            return I2C_ERR_UNKNOWN;
        }
        if (timeout-- == 0) return I2C_ERR_TIMEOUT; // Timeout
    }
    return I2C_OK;
}

I2C_Status_t I2C_Start(uint32_t timeout)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    return I2C_WaitEventUntilTimeout(I2C_EVENT_MASTER_MODE_SELECT, timeout);
}

I2C_Status_t I2C_Stop(uint32_t timeout)
{
    I2C_GenerateSTOP(I2C1, ENABLE);
    // 等待I2C总线空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        if (t-- == 0) return I2C_ERR_TIMEOUT; // Timeout
    }
    return I2C_OK;
}

I2C_Status_t I2C_SendAddr( uint8_t addr, uint8_t direction, uint32_t timeout)
{
    if (addr <= 0x7F) {                                                                                          // 参数错误
        if (direction != I2C_Direction_Transmitter && direction != I2C_Direction_Receiver) return I2C_ERR_PARAM; // 参数错误
        I2C_Send7bitAddress(I2C1, addr, direction);

        uint32_t t = timeout;
        // 检查是否进入EV6状态
        while (t--) {
            if (direction == I2C_Direction_Transmitter) {
                if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) return I2C_OK; // 发送模式
            } else {
                if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) return I2C_OK; // 接收模式
            }

            // 检查ACK失败标志AF
            if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != RESET) {
                I2C_ClearFlag(I2C1, I2C_FLAG_AF); // 清除ACK失败标志
                I2C_GenerateSTOP(I2C1, ENABLE);   // 发送停止信号
                return I2C_ERR_ACK_FAIL;          // 应答失败
            }

            // 其他错误
            if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR) != RESET) {
                I2C_ClearFlag(I2C1, I2C_FLAG_BERR); // 清除总线错误标志
                I2C_GenerateSTOP(I2C1, ENABLE);     // 发送停止信号
                return I2C_ERR_UNKNOWN;             // 未知错误
            }

            // ARLO 错误
            if (I2C_GetFlagStatus(I2C1, I2C_FLAG_ARLO) != RESET) {
                I2C_ClearFlag(I2C1, I2C_FLAG_ARLO); // 清除仲裁丢失标志
                I2C_GenerateSTOP(I2C1, ENABLE);     // 发送停止信号
                return I2C_ERR_UNKNOWN;             // 未知错误
            }
        }
        I2C_GenerateSTOP(I2C1, ENABLE); // 发送停止信号
        return I2C_ERR_TIMEOUT;         // Timeout
    } else {
        return I2C_ERR_PARAM; // 参数错误
    }
}


void OLED_WriteCommand(uint8_t cmd){
    I2C_Start(100);
    I2C_SendAddr(OLED_I2C_ADDR,I2C_Direction_Transmitter,100);
    I2C_SendData(I2C1,cmd);
    I2C_Stop(100);
}

void OLED_Init(void) {
    // 初始化命令序列
    OLED_WriteCommand(0xAE); // 关闭显示
    OLED_WriteCommand(0x20); // 设置内存寻址模式
    OLED_WriteCommand(0x10); // 页地址模式
    OLED_WriteCommand(0xB0); // 设置页起始地址
    OLED_WriteCommand(0xC8); // 设置COM扫描方向
    OLED_WriteCommand(0x00); // 设置低列地址
    OLED_WriteCommand(0x10); // 设置高列地址
    OLED_WriteCommand(0x40); // 设置起始行地址
    OLED_WriteCommand(0x81); // 设置对比度控制
    OLED_WriteCommand(0xFF); // 最大对比度
    OLED_WriteCommand(0xA1); // 设置段重映射
    OLED_WriteCommand(0xA6); // 设置正常显示
    OLED_WriteCommand(0xA8); // 设置多路复用比
    OLED_WriteCommand(0x3F); // 1/64 DUTY
    OLED_WriteCommand(0xA4); // 输出跟随RAM内容
    OLED_WriteCommand(0xD3); // 设置显示偏移
    OLED_WriteCommand(0x00); // 无偏移
    OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0xF0); // 设置分频比
    OLED_WriteCommand(0xD9); // 设置预充电周期
    OLED_WriteCommand(0x22); // 设置值
    OLED_WriteCommand(0xDA); // 设置COM引脚配置
    OLED_WriteCommand(0x12); // 配置值
    OLED_WriteCommand(0xDB); // 设置VCOMH消隐电平
    OLED_WriteCommand(0x20); // 配置值
    OLED_WriteCommand(0x8D); // 设置电荷泵
    OLED_WriteCommand(0x14); // 启用电荷泵
    OLED_WriteCommand(0xAF); // 打开显示
}

