#include "../inc/DMA.h"

void DMA_InitChannel(DMA_Config_t config){
    DMA_InitTypeDef DMA_InitStructure;
    
    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    DMA_InitStructure.DMA_PeripheralBaseAddr = config.DMA_PeripheralBaseAddr;
    DMA_InitStructure.DMA_MemoryBaseAddr = config.DMA_MemoryBaseAddr;
    DMA_InitStructure.DMA_DIR = config.DMA_DIR;
    DMA_InitStructure.DMA_BufferSize = config.DMA_BufferSize;
    DMA_InitStructure.DMA_PeripheralInc = config.DMA_PeripheralInc;
    DMA_InitStructure.DMA_MemoryInc = config.DMA_MemoryInc;
    DMA_InitStructure.DMA_PeripheralDataSize = config.DMA_PeripheralDataSize;
    DMA_InitStructure.DMA_MemoryDataSize = config.DMA_MemoryDataSize;
    DMA_InitStructure.DMA_Mode = config.DMA_Mode;
    DMA_InitStructure.DMA_Priority = config.DMA_Priority;
    DMA_InitStructure.DMA_M2M = config.DMA_M2M;
    
    DMA_Init(config.DMAy_Channelx, &DMA_InitStructure);
}


void DMA_StartTransfer(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t bufferSize){
    DMA_SetCurrDataCounter(DMAy_Channelx, bufferSize);
    DMA_Cmd(DMAy_Channelx, ENABLE);
}

void DMA_StopTransfer(DMA_Channel_TypeDef* DMAy_Channelx){
    DMA_Cmd(DMAy_Channelx, DISABLE);
}


bool DMA_IsTransferComplete(DMA_Channel_TypeDef* DMAy_Channelx){
    return DMA_GetFlagStatus(DMAy_Channelx == DMA1_Channel1 ? DMA1_FLAG_TC1 :
        DMAy_Channelx == DMA1_Channel2 ? DMA1_FLAG_TC2 :
        DMAy_Channelx == DMA1_Channel3 ? DMA1_FLAG_TC3 :
        DMAy_Channelx == DMA1_Channel4 ? DMA1_FLAG_TC4 :
        DMAy_Channelx == DMA1_Channel5 ? DMA1_FLAG_TC5 :
        DMAy_Channelx == DMA1_Channel6 ? DMA1_FLAG_TC6 :
        DMAy_Channelx == DMA1_Channel7 ? DMA1_FLAG_TC7 : 0);
}