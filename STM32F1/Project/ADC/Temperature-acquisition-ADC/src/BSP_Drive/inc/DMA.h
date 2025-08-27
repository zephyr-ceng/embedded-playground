#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f10x.h"

// DMA配置结构体
typedef struct {
    DMA_Channel_TypeDef* DMAy_Channelx; // DMA通道
    uint32_t DMA_PeripheralBaseAddr;    // 外设基地址
    uint32_t DMA_MemoryBaseAddr;        // 内存基地址
    uint16_t DMA_BufferSize;            // 缓冲区大小
    uint8_t DMA_DIR;                    // 传输方向
    uint8_t DMA_PeripheralInc;          // 外设地址增量
    uint8_t DMA_MemoryInc;              // 内存地址增量
    uint8_t DMA_PeripheralDataSize;     // 外设数据宽度
    uint8_t DMA_MemoryDataSize;         // 内存数据宽度
    uint8_t DMA_Mode;                   // 模式
    uint8_t DMA_Priority;               // 优先级
    uint8_t DMA_M2M;                    // 内存到内存模式
} DMA_Config_t;

void DMA_InitChannel(DMA_Config_t config);
void DMA_StartTransfer(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t bufferSize);
void DMA_StopTransfer(DMA_Channel_TypeDef* DMAy_Channelx);
bool DMA_IsTransferComplete(DMA_Channel_TypeDef* DMAy_Channelx);

#endif // !__DMA_H__
