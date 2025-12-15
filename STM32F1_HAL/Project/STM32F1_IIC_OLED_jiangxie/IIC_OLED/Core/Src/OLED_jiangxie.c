#include "OLED_jiangxie.h"
#include "OLED_Font_jiangxie.h"
#include "i2c.h"
#include "math.h"
#include "stdlib.h"

#define OLED_ADDRESS 0x78

// 向OLED发送数据
void OLED_Send(uint8_t *data, uint8_t len)
{
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, data, len, HAL_MAX_DELAY);
}

// 发送数据
void OLED_SendData(uint8_t data)
{
    static uint8_t sendBuffer[2] = {0x40, 0};
    sendBuffer[1] = data;
    OLED_Send(sendBuffer, 2);
}

// 向OLED发送指令
void OLED_SendCmd(uint8_t cmd)
{
    static uint8_t sendBuffer[2] = {0};
    sendBuffer[1] = cmd;
    OLED_Send(sendBuffer, 2);
}

// 初始化OLED
void OLED_Init(void)
{
    HAL_Delay(100);
    OLED_SendCmd(0xAE); /*关闭显示 display off*/

    OLED_SendCmd(0x20); // 设置内存地址模式
    OLED_SendCmd(0x10);

    OLED_SendCmd(0xB0); // 设置页地址

    OLED_SendCmd(0xC8); // 设置上下方向，0xC8正常 0xC0上下反置

    OLED_SendCmd(0x00); // 设置列地址低4位
    OLED_SendCmd(0x10); // 设置列地址高4位

    OLED_SendCmd(0x40); // 设置起始行地址

    OLED_SendCmd(0x81); // 设置对比度控制

    OLED_SendCmd(0xDF);
    OLED_SendCmd(0xA1); // 设置左右方向，0xA1正常 0xA0左右反置

    OLED_SendCmd(0xA6); // 设置正常/倒转显示
    OLED_SendCmd(0xA8); // 设置多路复用率

    OLED_SendCmd(0x3F); // 1/64占空比

    OLED_SendCmd(0xA4); // 设置整个显示打开/关闭

    OLED_SendCmd(0xD3); // 设置显示偏移
    OLED_SendCmd(0x00);

    OLED_SendCmd(0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_SendCmd(0xF0);

    OLED_SendCmd(0xD9); // 设置预充电周期
    OLED_SendCmd(0x22);

    OLED_SendCmd(0xDA); // 设置COM引脚硬件配置
    OLED_SendCmd(0x12);

    OLED_SendCmd(0xDB); // 设置VCOMH取消选择级别
    OLED_SendCmd(0x20);

    OLED_SendCmd(0x8D); // 设置充电泵
    OLED_SendCmd(0x14);

    OLED_SendCmd(0xAF); /*开启显示 display ON*/
}


/********************************************** 直接通信 **********************************************/
// 设置光标位置
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                 // 设置Y位置
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 设置X位置高4位
    OLED_WriteCommand(0x00 | (X & 0x0F));        // 设置X位置低4位
}

void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for (i = 0; i < 128; i++)
        {
            OLED_SendData(0x00);
        }
    }
}

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // 设置光标位置在上半部分
    for (i = 0; i < 8; i++)
    {
        OLED_SendData(OLED_F8x16[Char - ' '][i]); // 显示上半部分内容
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // 设置光标位置在下半部分
    for (i = 0; i < 8; i++)
    {
        OLED_SendData(OLED_F8x16[Char - ' '][i + 8]); // 显示下半部分内容
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
    for (i = 0; String[i] != '\0'; i++)
    {
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
    while (Y--)
    {
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
    for (i = 0; i < Length; i++)
    {
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
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)
    {
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
    for (i = 0; i < Length; i++)
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
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
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}


/********************************************** 带缓冲区的显示（操作缓存） **********************************************/
// TODO: 缓冲区设置
void OLED_NewFrame(void)



