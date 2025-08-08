#ifndef _SPI_DMA
#define _SPI_DMA
#include "stm32f10x.h"

void SPI_CSControl(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t state);
void SPI_ConfigInit(SPI_TypeDef *SPIx);
void SPI_Set_SendData(SPI_TypeDef *SPIx, uint16_t data, uint16_t len);
void SPI_Get_ReceiveData(uint16_t *data);

#endif // !_SPI_DMA