#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f10x.h"
#include "stdbool.h"

typedef enum {
    ADC1,
    ADC2,
    USART1,
    USART2
    // TODO: 可以继续添加外设类型，同时需要更改DMA_EnablePeripheral中的case,两两对应
} DMA_PeripheralType;

// DMA配置结构体 在stm32f10x_dma.h中的定义为uint32_t
typedef struct {
    DMA_PeripheralType Peripheral;      // 外设类型
    DMA_Channel_TypeDef *DMAy_Channelx; // DMA通道
    uint32_t DMA_PeripheralBaseAddr;    // 外设基地址
    uint32_t DMA_MemoryBaseAddr;        // 内存基地址
    uint32_t DMA_BufferSize;            // 缓冲区大小
    uint32_t DMA_DIR;                    // 传输方向
    uint32_t DMA_PeripheralInc;          // 外设地址增量
    uint32_t DMA_MemoryInc;              // 内存地址增量
    uint32_t DMA_PeripheralDataSize;     // 外设数据宽度
    uint32_t DMA_MemoryDataSize;         // 内存数据宽度
    uint32_t DMA_Mode;                   // 模式
    uint32_t DMA_Priority;               // 优先级
    uint32_t DMA_M2M;                    // 内存到内存模式
} DMA_Config_t;

// DMA_Config_t DMA_Config = {
//     DMA1_Channel4, USART1, RxBuffer,Rx_BufferSize DMA_DIR_PeripheralSRC, DMA_PeripheralInc_Disable, DMA_MemoryInc_Enable, DMA_Mode_Normal, DMA_Priority_High, DMA_M2M_Disable};
void DMA_InitChannel(DMA_Config_t config);
void DMA_StartTransfer(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t bufferSize);
void DMA_StopTransfer(DMA_Channel_TypeDef *DMAy_Channelx);
bool DMA_IsTransferComplete(DMA_Channel_TypeDef *DMAy_Channelx);
void DMA_EnablePeripheral(DMA_Config_t config, FunctionalState state);

#endif // !__DMA_H__


