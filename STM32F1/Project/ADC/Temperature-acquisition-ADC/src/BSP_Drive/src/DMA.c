#include "../inc/DMA.h"
#include "string.h"

/**
 * @brief  初始化DMA1,外设到内存的传输
 * @param  config: DMA初始化结构体,Peripheral-外设类型,
 * @retval Null
 * */
void DMA_InitChannel_PtoM(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    if (*m) return;

    DMA_InitTypeDef DMA_InitStructure;

    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(m->channel);
    DMA_Cmd(m->channel, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = m->peripheral_addr;                  // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)config.DMA_MemoryBaseAddr; // 内存地址
    DMA_InitStructure.DMA_BufferSize         = (uint32_t)config.DMA_BufferSize;     // 传输大小
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;           // 外设增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;         // 外设数据宽度
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;             // 内存数据宽度
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;                // 内存增量
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;               // 外设到存储器
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;                   // 循环传输
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;                 // 多通道涉及优先级
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                     // 存储器到存储器使用

    DMA_Init(m->channel, &DMA_InitStructure);
    // DMA_EnablePeripheral(config,ENABLE); // 防止外设未初始化进行调用，所以需要显式的外部调用
}

/**
 * @brief  初始化DMA1,内存到外设
 * @param  config: DMA初始化结构体
 * @retval Null
 * */
DMA_Status_e DMA_InitChannel_MtoP(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    if (m == NULL) {
        return DMA_ERR_NOT_FOUND;
    }

    DMA_InitTypeDef DMA_InitStructure;

    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitStructure.DMA_PeripheralBaseAddr = m->peripheral_addr;          // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr     = config.DMA_MemoryBaseAddr;   // 内存地址
    DMA_InitStructure.DMA_BufferSize         = config.DMA_BufferSize;       // 传输大小
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   // 外设增量
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;     // 内存数据宽度
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;        // 内存增量
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;       // 内存到外设
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;             // 单次传输
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;         // 多通道涉及优先级
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;             // 存储器到存储器使用

    DMA_Init(m->channel, &DMA_InitStructure);
    // DMA_EnablePeripheral(config,ENABLE); // 防止外设未初始化进行调用，所以需要显式的外部调用
    return DMA_OK;
}

static const DMA_Mapping_t *DMA_Mapping_Find(DMA_Peripheral_e Peripheral)
{
    for (size_t i = 0; i < dma_mapping_sz; ++i) {
        if (dma_mapping[i].Peripheral == Peripheral) return &dma_mapping[i];
    }
    return NULL;
}

/**
 * @brief  外设类型的DMA服务启动
 * @param  config: DMA初始化结构体
 * @param  state: enable-启动，disable-停止
 * @retval Null
 * */
void DMA_EnablePeripheral(DMA_Config_t config, FunctionalState State)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    if (*m) return;
    switch (config.peripheral) {
        case DMA_PERIPH_ADC1:
            ADC_DMACmd(ADC1, State);
            break;
        case DMA_PERIPH_ADC2:
            ADC_DMACmd(ADC2, State);
            break;
        case DMA_PERIPH_USART1_TX:
            USART_DMACmd(USART1, (uint16_t)m->request_flag, State);
            break;

        case DMA_PERIPH_USART1_RX:
            USART_DMACmd(USART1, (uint16_t)m->request_flag, State);
            break;
        case DMA_PERIPH_USART2_TX:
            USART_DMACmd(USART2, (uint16_t)m->request_flag, State);
            break;
        case DMA_PERIPH_USART2_RX:
            USART_DMACmd(USART2, (uint16_t)m->request_flag, State);
            break;
        case DMA_PERIPH_SPI1_TX:
            SPI_I2S_DMACmd(SPI1, (uint16_t)m->request_flag, State);
            break;
        case DMA_PERIPH_SPI1_TX:
            SPI_I2S_DMACmd(SPI1, (uint16_t)m->request_flag, State);
            break;
            // 可以继续添加case
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
