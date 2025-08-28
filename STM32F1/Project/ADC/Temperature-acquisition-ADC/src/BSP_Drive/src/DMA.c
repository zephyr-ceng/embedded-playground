#include "../inc/DMA.h"
#include "string.h"



/**
 * @brief  初始化DMA1
 * @param  config: DMA初始化结构体
 * @retval Null
 * */
void DMA_InitChannel(DMA_Config_t config)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitStructure.DMA_PeripheralBaseAddr = config.DMA_PeripheralBaseAddr;
    DMA_InitStructure.DMA_MemoryBaseAddr     = config.DMA_MemoryBaseAddr;
    DMA_InitStructure.DMA_DIR                = config.DMA_DIR;
    DMA_InitStructure.DMA_BufferSize         = config.DMA_BufferSize;
    DMA_InitStructure.DMA_PeripheralInc      = config.DMA_PeripheralInc;
    DMA_InitStructure.DMA_MemoryInc          = config.DMA_MemoryInc;
    DMA_InitStructure.DMA_PeripheralDataSize = config.DMA_PeripheralDataSize;
    DMA_InitStructure.DMA_MemoryDataSize     = config.DMA_MemoryDataSize;
    DMA_InitStructure.DMA_Mode               = config.DMA_Mode;
    DMA_InitStructure.DMA_Priority           = config.DMA_Priority;
    DMA_InitStructure.DMA_M2M                = config.DMA_M2M;

    DMA_Init(config.DMAy_Channelx, &DMA_InitStructure);
    // DMA_EnablePeripheral(config,ENABLE); // 防止外设未初始化进行调用，所以需要显式的外部调用
}



void DMA_InitChannel(DMA_Config_t config)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1 ->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    

}



/**
* @brief  外设类型的DMA服务启动
* @param  config: DMA初始化结构体
* @param  state: enable-启动，disable-停止
* @retval 
* */

void DMA_EnablePeripheral(DMA_Config_t config, FunctionalState state){
    switch (config.peripheral) {
        case ADC1:
            ADC_DMACmd(ADC1, state);
            break;
        case ADC2:
            ADC_DMACmd(ADC2, state);
            break;
        case USART1:
            USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, state);
            break;

        case USART2:
            USART_DMACmd(USART2, USART_DMAReq_Tx | USART_DMAReq_Rx, state);
            break;
            // TODO: 可以继续添加case
    }
}

/**
 * @brief  设置缓冲区大小并重启DMA服务
 * @param  DMAy_Channelx: DMA通道
 * @param  bufferSize: 设置缓冲区大小
 * @retval Null
 * */
void DMA_StartTransfer(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t bufferSize)
{
    DMA_SetCurrDataCounter(DMAy_Channelx, bufferSize);
    DMA_Cmd(DMAy_Channelx, ENABLE);
}

/**
 * @brief  停止DMA服务
 * @param  DMAy_Channelx: DMA通道
 * @retval Null
 * */
bool DMA_StopTransfer(DMA_Channel_TypeDef *DMAy_Channelx)
{
    uint32_t flag;
    DMA_Cmd(DMAy_Channelx, DISABLE);
    switch (DMAy_Channelx) {
        case DMA1_Channel1:
            flag = DMA1_FLAG_TC1;
            break;
        case DMA1_Channel2:
            flag = DMA1_FLAG_TC2;
            break;
        case DMA1_Channel3:
            flag = DMA1_FLAG_TC3;
            break;
        case DMA1_Channel4:
            flag = DMA1_FLAG_TC4;
            break;
        case DMA1_Channel5:
            flag = DMA1_FLAG_TC5;
            break;
        case DMA1_Channel6:
            flag = DMA1_FLAG_TC6;
            break;
        case DMA1_Channel7:
            flag = DMA1_FLAG_TC7;
            break;
        default:
            flag = 0;
    }
    if (!flag) {
        DMA_ClearFlag(flag);
    }
}

/**
 * @brief  DMA 通道传输状态检测
 * @param  DMAy_Channelx: DMA通道
 * @retval True: 传输完成
 * @retval False: 传输失败
 * */
bool DMA_IsTransferComplete(DMA_Channel_TypeDef *DMAy_Channelx)
{
    uint32_t flag;
    switch (DMAy_Channelx) {
        case DMA1_Channel1:
            flag = DMA1_FLAG_TC1;
            break;
        case DMA1_Channel2:
            flag = DMA1_FLAG_TC2;
            break;
        case DMA1_Channel3:
            flag = DMA1_FLAG_TC3;
            break;
        case DMA1_Channel4:
            flag = DMA1_FLAG_TC4;
            break;
        case DMA1_Channel5:
            flag = DMA1_FLAG_TC5;
            break;
        case DMA1_Channel6:
            flag = DMA1_FLAG_TC6;
            break;
        case DMA1_Channel7:
            flag = DMA1_FLAG_TC7;
            break;
        default:
            if (flag == 0) return FALSE
    }

    return (DMA_GetFlagStatus(flag) != RESET); // TRUE 表示传输完成
}
