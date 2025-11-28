#ifndef __SPI_H__
#define __SPI_H__

#include "./main.h"

#define SPI1_CLK GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_PORT GPIOA



void KeyBoard_SPI_Init();

#endif // !__SPI_H__