#include "spi.h"


void SPI1_GPIO_Init(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef gpio_init_structure;
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AF_PP; // 推挽输出
    gpio_init_structure.GPIO_Pin = SPI1_CLK;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI1_PORT,&gpio_init_structure);
    
    gpio_init_structure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    gpio_init_structure.GPIO_Pin = SPI1_MISO;
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI1_PORT,&gpio_init_structure);
}


// 编码器
void SPI1_Config(){
    SPI_InitTypeDef spi_init_structure;
    DMA_InitTypeDef dma_init_structure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    // SPI1初始化
    spi_init_structure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // 双线全双工
    spi_init_structure.SPI_Mode = SPI_Mode_Master; // 主模式，产生clk时钟
    spi_init_structure.SPI_DataSize = SPI_DataSize_8b; // 传输数据大小 8bit
    spi_init_structure.SPI_CPOL = SPI_CPOL_High; // 空闲高电平
    spi_init_structure.SPI_CPHA = SPI_CPHA_1Edge; // 第一个时钟边沿采样
    spi_init_structure.SPI_NSS = SPI_NSS_Soft; // 软件片选
    spi_init_structure.SPI_FirstBit = SPI_FirstBit_MSB; // 高位先传
    spi_init_structure.SPI_CRCPolynomial = DISABLE; // 禁用多项式传输
    spi_init_structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // 16分频
    SPI_Init(SPI1,&spi_init_structure);
    SPI_Cmd(SPI1,ENABLE);


    // DMA_RX
    DMA_DeInit(DMA1_Channel2);
    dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
    dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度
    dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC; // 外设到内存
    dma_init_structure.DMA_MemoryBaseAddr = rx_buffer;
    dma_init_structure.DMA_BufferSize = rx_buffer_size;
    dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据宽度
    dma_init_structure.DMA_Mode = DMA_Mode_Normal; // 普通单次传输模式
    dma_init_structure.DMA_Priority = DMA_Priority_High; // 高优先级
    dma_init_structure.DMA_M2M  = DMA_M2M_Disable; // 禁用内存到内存
    

}

