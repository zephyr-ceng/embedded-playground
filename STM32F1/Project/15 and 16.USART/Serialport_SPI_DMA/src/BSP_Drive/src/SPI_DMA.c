#include "./BSP_Drive/inc/SPI_DMA.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "string.h"

#define SPI_TxBufferSize 256 // SPI TX buffer size
#define SPI_RxBufferSize 256 // SPI RX buffer size
// uint8_t SPI_RxBufferSize = 256; // SPI RX buffer size
// uint8_t SPI_TxBufferSize = 256; // SPI TX buffer size
uint16_t SPI_TxBuffer[SPI_TxBufferSize]; // SPI TX buffer
uint16_t SPI_RxBuffer[SPI_RxBufferSize]; // SPI RX buffer

void SPI_GPIOConfig(SPI_TypeDef *SPIx)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (SPIx == SPI1) {
        // Enable the GPIO clock for SPI1
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        // Configure SPI1 pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // SCK, MOSI, MISO
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;                      // Alternate function push-pull
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    if (SPIx == SPI2) {
        // Enable the GPIO clock for SPI2
        RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        // Configure SPI2 pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; // SCK, MOSI, MISO
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

void SPI_ConfigInit(SPI_TypeDef *SPIx)
{
    SPI_InitTypeDef SPI;

    // Enable the SPI peripheral clock
    if (SPIx == SPI1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    }
    if (SPIx == SPI2) {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }

    // Initialize the SPI configuration structure
    SPI_StructInit(&SPI);
    // Configure the SPI parameters
    SPI.SPI_Direction         = SPI_Direction_2Lines_FullDuplex; // 传输模式为全双工
    SPI.SPI_Mode              = SPI_Mode_Master;                 // 主模式
    SPI.SPI_DataSize          = SPI_DataSize_8b;                 // 数据大小为8位
    SPI.SPI_CPOL              = SPI_CPOL_Low;                    // 时钟极性低
    SPI.SPI_CPHA              = SPI_CPHA_1Edge;                  // 时钟相位为第1个边沿
    SPI.SPI_NSS               = SPI_NSS_Soft;                    // NSS信号由软件管理
    SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;        // 波特率预分频器为16
    SPI.SPI_FirstBit          = SPI_FirstBit_MSB;                // 数据传输从MSB开始
    SPI_Init(SPIx, &SPI);
}

void SPI_DMA_TxConfigInit(SPI_TypeDef *SPIx)
{
    DMA_InitTypeDef dma;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 开启DMA1时钟

    if (SPIx == SPI1) {
        DMA_DeInit(DMA1_Channel3);
        dma.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
        dma.DMA_MemoryBaseAddr     = (uint32_t)SPI_TxBuffer;      // 稍后设置
        dma.DMA_DIR                = DMA_DIR_PeripheralDST;       // 外设到内存传输
        dma.DMA_BufferSize         = SPI_TxBufferSize;            // 稍后设置
        dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
        dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
        dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小为字节
        dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存数据大小为字节
        dma.DMA_Mode               = DMA_Mode_Normal;             // 普通模式
        dma.DMA_Priority           = DMA_Priority_High;           // 高优先级
        dma.DMA_M2M                = DMA_M2M_Disable;             // 禁用内存到内存传输
        DMA_Init(DMA1_Channel3, &dma);
    }
    if (SPIx == SPI2) {
        DMA_DeInit(DMA1_Channel5);
        dma.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
        dma.DMA_MemoryBaseAddr     = (uint32_t)SPI_TxBuffer;      // 稍后设置
        dma.DMA_DIR                = DMA_DIR_PeripheralDST;       // 外设到内存传输
        dma.DMA_BufferSize         = SPI_TxBufferSize;            // 稍后设置
        dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
        dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
        dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小为字节
        dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存数据大小为字节
        dma.DMA_Mode               = DMA_Mode_Normal;             // 普通模式
        dma.DMA_Priority           = DMA_Priority_High;           // 高优先级
        dma.DMA_M2M                = DMA_M2M_Disable;             // 禁用内存到内存传输
        DMA_Init(DMA1_Channel5, &dma);
    }

    // 开启 SPI 的 DMA 功能
    SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);
}

void SPI_DMA_RxConfigInit(SPI_TypeDef *SPIx)
{
    DMA_InitTypeDef dma;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // 开启DMA1时钟

    if (SPIx == SPI1) {
        DMA_DeInit(DMA1_Channel2);
        dma.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
        dma.DMA_MemoryBaseAddr     = (uint32_t)SPI_RxBuffer;      // 稍后设置
        dma.DMA_DIR                = DMA_DIR_PeripheralSRC;       // 内存到外设传输
        dma.DMA_BufferSize         = SPI_RxBufferSize;            // 稍后设置
        dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
        dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
        dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小为字节
        dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存数据大小为字节
        dma.DMA_Mode               = DMA_Mode_Normal;             // 普通模式
        dma.DMA_Priority           = DMA_Priority_High;           // 高优先级
        dma.DMA_M2M                = DMA_M2M_Disable;             // 禁用内存到内存传输
        DMA_Init(DMA1_Channel2, &dma);
    }
    if (SPIx == SPI2) {
        DMA_DeInit(DMA1_Channel4);
        dma.DMA_PeripheralBaseAddr = (uint32_t)&SPIx->DR;
        dma.DMA_MemoryBaseAddr     = (uint32_t)SPI_RxBuffer;      // 稍后设置
        dma.DMA_DIR                = DMA_DIR_PeripheralSRC;       // 内存到外设传输
        dma.DMA_BufferSize         = SPI_RxBufferSize;            // 稍后设置
        dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设地址不递增
        dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存地址递增
        dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据大小为字节
        dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存数据大小为字节
        dma.DMA_Mode               = DMA_Mode_Normal;             // 普通模式
        dma.DMA_Priority           = DMA_Priority_High;           // 高优先ity
        dma.DMA_M2M                = DMA_M2M_Disable;             // 禁用内存到内存传输
        DMA_Init(DMA1_Channel4, &dma);
    }
    SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);
}

void SPI_NVIC_ConfigInit(SPI_TypeDef *SPIx)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    if (SPIx == SPI1) {
        NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    }
    if (SPIx == SPI2) {
        NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    }
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;      // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;      // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE; // 使能中断
    NVIC_Init(&NVIC_InitStructure);
}

// TODO: SPI1 和 SPI2 的中断处理函数还未实现,需要根据传输的实际大小更改RxBufferSize
/* void SPI1_IRQnhandler(void) 
{
    // 处理SPI1中断
    if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) != RESET) {
        // Clear the RXNE interrupt flag
        SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
        // 接收数据
        SPI_RxBuffer     = SPI_I2S_ReceiveData(SPI1);
        SPI_RxBufferSize = DMA_GetCurrDataCounter(DMA1_Channel2); // 获取当前接收数据计数
    }
} */

void SPI_Config_Init(SPI_TypeDef *SPIx)
{
    SPI_GPIOConfig(SPIx);
    SPI_ConfigInit(SPIx);
    SPI_DMA_RxConfigInit(SPIx);
    SPI_DMA_TxConfigInit(SPIx);
}

// TODO: 片选端口暂未固定，需要根据实际硬件连接进行配置
void SPI_CSControl(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t state)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // Configure the GPIO pin for CS
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; // Push-pull output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    // Set or reset the CS pin
    if (state == 0) {
        GPIO_ResetBits(GPIOx, GPIO_Pin); // Select (CS low)
    } else {
        GPIO_SetBits(GPIOx, GPIO_Pin); // Deselect (CS high)
    }
}

void SPI_Set_SendData(SPI_TypeDef *SPIx, uint16_t data, uint16_t len)
{
    if (len > SPI_TxBufferSize) {
        len = SPI_TxBufferSize; // Ensure we do not exceed the buffer size
    }
    for (int i = 0; i < len; i++) {
        SPI_TxBuffer[i] = data; // Fill the TX buffer with the data
    }
    if (SPIx == SPI1) {
        DMA_Cmd(DMA1_Channel3, DISABLE);            // Enable DMA for SPI1 TX
        DMA_SetCurrDataCounter(DMA1_Channel3, len); // Set the number of data to be sent
        DMA_Cmd(DMA1_Channel3, ENABLE);             // Enable DMA for SPI1 TX
    } else if (SPIx == SPI2) {
        DMA_Cmd(DMA1_Channel5, DISABLE);            // Enable DMA for SPI2 TX
        DMA_SetCurrDataCounter(DMA1_Channel5, len); // Set the number of data to be sent
        DMA_Cmd(DMA1_Channel5, ENABLE);             // Enable DMA for SPI2 TX
    }
}

void SPI_Get_ReceiveData(uint16_t *data)
{
    for (int i = 0; i < SPI_RxBufferSize; i++) {
        data[i] = SPI_RxBuffer[i]; // Copy received data to output buffer
    }
    memset(SPI_RxBuffer, 0, sizeof(SPI_RxBuffer)); // Clear the RX buffer
    // TODO: BufferSize 问题处理，大小不可更改
    // SPI_TxBufferSize = 256;                        // Reset RX buffer size

}
