#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f10x.h"
#include "stdbool.h"

typedef enum {
    DMA_OK = 0,
    DMA_ERR_NOT_FOUND = -1,
    DMA_ERR_NOT_BUFFER = -2
} DMA_Status_e;

typedef enum {
    DMA_PERIPH_ADC1,
    DMA_PERIPH_ADC2,
    DMA_PERIPH_USART1_TX,
    DMA_PERIPH_USART1_RX,
    DMA_PERIPH_USART2_TX,
    DMA_PERIPH_USART2_RX,
    DMA_PERIPH_SPI1_TX,
    DMA_PERIPH_SPI1_RX
    // 持续添加
} DMA_Peripheral_e;

typedef struct {       
    DMA_Channel_TypeDef *channel; // DMA通道
    uint32_t request_flag;        // 请求标志
    uint32_t peripheral_addr;     // 外设基地址
    uint32_t flagStatus;        // 标志位
} DMA_Mapping_t;

const DMA_Mapping_t dma_mapping[] = {
    {DMA_PERIPH_ADC1, DMA1_Channel1, 0, (uint32_t)&ADC1->DR,DMA1_FLAG_TC1},
    {DMA_PERIPH_ADC2, DMA1_Channel2, 0, (uint32_t)&ADC2->DR,DMA1_FLAG_TC2},
    {DMA_PERIPH_USART1_TX, DMA1_Channel4, USART_DMAReq_Tx, (uint32_t)&USART1->DR,DMA1_FLAG_TC4},
    {DMA_PERIPH_USART1_RX, DMA1_Channel5, USART_DMAReq_Rx, (uint32_t)&USART1->DR,DMA1_FLAG_TC5},
    {DMA_PERIPH_USART2_TX, DMA1_Channel7, USART_DMAReq_Tx, (uint32_t)&USART2->DR,DMA1_FLAG_TC7},
    {DMA_PERIPH_USART2_RX, DMA1_Channel6, USART_DMAReq_Rx, (uint32_t)&USART2->DR,DMA1_FLAG_TC6},
    {DMA_PERIPH_SPI1_TX, DMA1_Channel3, SPI_I2S_DMAReq_Tx, (uint32_t)&SPI1->DR,DMA1_FLAG_TC3},
    {DMA_PERIPH_SPI1_RX, DMA1_Channel2, SPI_I2S_DMAReq_Rx, (uint32_t)&SPI1->DR,DMA1_FLAG_TC2}};

static const size_t dma_mapping_sz = sizeof(dma_mapping) / sizeof(dma_mapping[0]); 

typedef struct {
    DMA_Peripheral_e Peripheral; // 外设类型
    uint32_t DMA_MemoryBaseAddr; // 缓冲区地址，内存基地址
    uint32_t DMA_BufferSize;     // 缓冲区大小
} DMA_Config_t;

void DMA_InitChannel_PtoM(DMA_Config_t config);
void DMA_InitChannel_MtoP(DMA_Config_t config);
void DMA_EnablePeripheral(DMA_Config_t config, FunctionalState NewState);
void DMA_StartTransfer(DNA_config_t config);
void DMA_StopTransfer(DMA_Config_t config);
bool DMA_IsTransferComplete(DMA_Config_t config);
uint32_t DMA_GetCurrDataSize(DMA_Config_t config);
#endif // !__DMA_H__
