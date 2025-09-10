/****************
 *@description: SPI驱动源文件
 *@author: zephyr
 *@date: 2025-09-08 16:18:06
 *@version: V1.0.0
 ****************/

#include "./BSP_Drive/inc/SPI.h"


// SPI初始化
void SPI_InitModule(SPI_Config_t config)
{
    // 1. 使能SPI时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // 3. 配置SPI参数
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex; // 全双工
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                 // 主模式
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                 // 8位数据帧
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                    // 时钟空闲低电平
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;                  // 第一个时钟边沿采样
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                    // 软件管理NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = config.BaudRatePrescaler;        // 波特率预分频16
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                // MSB先行
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                               // CRC多项式

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE); // 使能SPI
}








