#include "stm32f10x.h"
#include "OLED_IIC.h"
#include "string.h"
#include "./drive/src/font.h"

/* STM32自身的I2C地址，这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 0x00
/* STM32 I2C 速率 */
#define I2C_Speed 400000

/*通讯等待超时时间*/
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/****************************************  初始化 *******************************************/
void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // Configure SCL (PB6) and SDA (PB7) as open-drain outputs
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD; // Open-drain mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void I2C_Mode_Config(void)
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

void I2C_Init(void)
{
    I2C_GPIO_Config(); // Initialize GPIO for I2C
    I2C_Mode_Config(); // Configure I2C mode
}

/****************************************  I2C基本功能实现 *******************************************/
// I2C 传输超时判定（0：timeout 1 success）
uint8_t I2C_waitEvent(I2C_TypeDef *I2Cx, uint32_t event)
{
    uint32_t timeout = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, event))
    {
        if (timeout-- == 0)
        {
            return 0; // Timeout
        }
    }
    return 1;
}


void I2C_Start(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTART(I2Cx, ENABLE); 
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)); 
}

void I2C_Stop(I2C_TypeDef *I2Cx)
{
    I2C_GenerateSTOP(I2Cx, ENABLE); 
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF)); 
}

/**
* @brief  发送I2C地址
* @param  I2Cx: 指向I2C外设的指针
* @param  addr: 7位I2C地址
* @param  direction: I2C方向（I2C_Direction_Transmitter 或 I2C_Direction_Receiver）
* @retval Null
* */
void I2C_SendAddr(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t direction)
{
    I2C_Send7bitAddress(I2Cx, addr, direction);
    if (direction == I2C_Direction_Transmitter)
        while (EEPROM_IIC_WaitEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    else
        while (EEPROM_IIC_WaitEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
}

void I2C_SendByte(I2C_TypeDef *I2Cx, uint8_t data)
{
    I2C_SendData(I2Cx, data); 
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
}





