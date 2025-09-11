/****************
 *@description: SPI驱动头文件
 *@author: zephyr
 *@date: 2025-09-08 16:17:24
 *@version: V1.0.0
 ****************/

#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f10x.h"
#include "stm32f10x_spi.h"

typedef enum {
    SPI_BRP_2   = SPI_BaudRatePrescaler_2,
    SPI_BRP_4   = SPI_BaudRatePrescaler_4,
    SPI_BRP_8   = SPI_BaudRatePrescaler_8,
    SPI_BRP_16  = SPI_BaudRatePrescaler_16,
    SPI_BRP_32  = SPI_BaudRatePrescaler_32,
    SPI_BRP_64  = SPI_BaudRatePrescaler_64,
    SPI_BRP_128 = SPI_BaudRatePrescaler_128,
    SPI_BRP_256 = SPI_BaudRatePrescaler_256
} SPI_BaudRatePrescaler_t;

typedef struct {
    SPI_BaudRatePrescaler_t BaudRatePrescaler; // 波特率预分频
} SPI_Config_t;

void SPI_InitModule(SPI_Config_t config);

#endif // !__SPI_H__