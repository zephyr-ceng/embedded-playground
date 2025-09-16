#include "./Module/inc/OLED_IIC.h"
#include "./Module/inc/OLED_Font.h"

#define I2C_Speed        400000 // 100kHz standard mode
#define I2C_OWN_ADDRESS7 0x00   // 本机地址
#define OLED_I2C_ADDR    0x78   // 八位地址

void I2C_InitConfiguration()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    // GPIO初始化
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD; // 设置GPIO模式为开漏输出
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

// 超时时间判定
I2C_Status_t I2C_WaitEvent(uint32_t event, uint32_t timeout)
{
    while (!I2C_CheckEvent(I2C2, event)) {
        if (timeout-- == 0) return I2C_ERR_TIMEOUT; // Timeout
        if (I2C_GetFlagStatus(I2C2, I2C_FLAG_AF) == SET) {
            /* 清 AF 并返回 ACK 失败 */
            I2C_ClearFlag(I2C2, I2C_FLAG_AF);
            return I2C_ERR_ACK_FAIL;
        }
        if (I2C_GetFlagStatus(I2C2, I2C_FLAG_BERR) == SET) {
            I2C_ClearFlag(I2C2, I2C_FLAG_BERR);
            return I2C_ERR_UNKNOWN;
        }
        if (I2C_GetFlagStatus(I2C2, I2C_FLAG_ARLO) == SET) {
            I2C_ClearFlag(I2C2, I2C_FLAG_ARLO);
            return I2C_ERR_UNKNOWN;
        }
    }
    return I2C_OK;
}

I2C_Status_t I2C_Start(uint32_t timeout)
{
    I2C_GenerateSTART(I2C2, ENABLE);
    return I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, timeout);
}

I2C_Status_t I2C_Stop()
{
    I2C_GenerateSTOP(I2C2, ENABLE);
}

I2C_Status_t I2C_SendAddr(uint8_t addr, uint32_t direction, uint32_t timeout)
{
    if ((addr & 0xFE) != addr) addr >>= 1; // 确保地址为七位
    I2C_Send7bitAddress(I2C2, addr, direction);
    if (direction == I2C_Direction_Transmitter) {
        return I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10);
    } else if (direction == I2C_Direction_Receiver) {
        return I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, 10);
    } else {
        return I2C_ERR_PARAM; // 参数错误
    }
}

I2C_Status_t I2C_SendDataByte(uint8_t data)
{
    I2C_SendData(I2C2, data);
    return I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, 10);
}

uint8_t I2C_ReadDataByte(uint8_t ack)
{
    if (ack)
        I2C_AcknowledgeConfig(I2C2, ENABLE);
    else
        I2C_AcknowledgeConfig(I2C2, DISABLE);

    I2C_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
    return I2C_ReceiveData(I2C2);
}

// 
void OLED_WriteCommand(uint8_t cmd)
{
    // SS1306 I2C传输协议要求传输顺序：设备地址，控制字节，命令
    (void)I2C_Start(100);                                              // 起始信号
    (void)I2C_SendAddr(OLED_I2C_ADDR, I2C_Direction_Transmitter, 100); // 设备地址
    (void)I2C_SendDataByte(0x00);                                      // 0x00=控制字节，0x40=数据
    (void)I2C_SendDataByte(cmd);                                       // 命令
    (void)I2C_Stop();                                                  // 停止信号
    // 使用void 可忽略错误，不需要传入底层服务，可以保留错误信息，调试和日志记录
}

void OLED_WriteData(uint8_t data)
{
    I2C_Start(100);
    I2C_SendAddr(OLED_I2C_ADDR, I2C_Direction_Transmitter, 100);
    I2C_SendDataByte(0x40);
    I2C_SendDataByte(data);
    I2C_Stop();
}

void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                 // 设置Y位置
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));        // 设置X位置低4位
}

/**
 * @brief  OLED清屏
 * @param  无
 * @retval 无
 */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++) {
        OLED_SetCursor(j, 0);
        for (i = 0; i < 128; i++) {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_Init(void)
{
    uint32_t i, j;

    for (i = 0; i < 1000; i++) // 上电延时
    {
        for (j = 0; j < 1000; j++);
    }

    I2C_InitConfiguration(); // I2C初始化
    OLED_WriteCommand(0xAE); // 关闭显示
    OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xA8); // 设置多路复用率
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3); // 设置显示偏移
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40); // 设置显示开始行
    OLED_WriteCommand(0xA1); // 设置左右方向，0xA1正常 0xA0左右反置
    OLED_WriteCommand(0xC8); // 设置上下方向，0xC8正常 0xC0上下反置
    OLED_WriteCommand(0xDA); // 设置COM引脚硬件配置
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81); // 设置对比度控制
    OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xD9); // 设置预充电周期
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDB); // 设置VCOMH取消选择级别
    OLED_WriteCommand(0x30);
    OLED_WriteCommand(0xA4); // 设置整个显示打开/关闭
    OLED_WriteCommand(0xA6); // 设置正常/倒转显示
    OLED_WriteCommand(0x8D); // 设置充电泵
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xAF); // 开启显示
    OLED_Clear();            // OLED清屏
}

/**
 * @brief  OLED显示一个字符
 * @param  Line 行位置，范围：1~4
 * @param  Column 列位置，范围：1~16
 * @param  Char 要显示的一个字符，范围：ASCII可见字符
 * @retval 无
 */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // 设置光标位置在上半部分
    for (i = 0; i < 8; i++) {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]); // 显示上半部分内容
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // 设置光标位置在下半部分
    for (i = 0; i < 8; i++) {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]); // 显示下半部分内容
    }
}

/**
 * @brief  OLED显示字符串
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  String 要显示的字符串，范围：ASCII可见字符
 * @retval 无
 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/**
 * @brief  OLED次方函数
 * @retval 返回值等于X的Y次方
 */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--) {
        Result *= X;
    }
    return Result;
}

/**
 * @brief  OLED显示数字（十进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~4294967295
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED显示数字（十进制，带符号数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：-2147483648~2147483647
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0) {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    } else {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++) {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * @brief  OLED显示数字（十六进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
 * @param  Length 要显示数字的长度，范围：1~8
 * @retval 无
 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++) {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10) {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        } else {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
 * @brief  OLED显示数字（二进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
 * @param  Length 要显示数字的长度，范围：1~16
 * @retval 无
 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}
