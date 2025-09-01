#include "../inc/DMA.h"
#include "string.h"

/**
 * @brief  初始化DMA1,外设到内存的传输
 * @param  config: DMA初始化结构体,Peripheral-外设类型,DMA_MemoryBaseAddr-内存基地址,DMA_BufferSize-缓冲区大小
 * @retval DMA_Status_e: DMA_OK-成功, DMA_ERR_NOT_FOUND-未找到对应外设, DMA_ERR_NOT_BUFFER-缓冲区错误
 * */
DMA_Status_e DMA_InitChannel_PtoM(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    if (m == NULL) return DMA_ERR_NOT_FOUND;

    // Buffer的非法性检查
    if (config.DMA_MemoryBaseAddr == 0 || config.DMA_BufferSize == 0) return DMA_ERR_NOT_BUFFER;

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
    return DMA_OK;
}

/**
 * @brief  初始化DMA1,内存到外设的传输
 * @param  config: DMA初始化结构体,Peripheral-外设类型,DMA_MemoryBaseAddr-内存基地址,DMA_BufferSize-缓冲区大小
 * @retval DMA_Status_e: DMA_OK-成功, DMA_ERR_NOT_FOUND-未找到对应外设, DMA_ERR_NOT_BUFFER-缓冲区地址或大小非法
 * */
DMA_Status_e DMA_InitChannel_MtoP(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    if (m == NULL) return DMA_ERR_NOT_FOUND;

    // Buffer的非法性检查
    if (config.DMA_MemoryBaseAddr == 0 || config.DMA_BufferSize == 0) return DMA_ERR_NOT_BUFFER;

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
    return DMA_OK;
}

/**
 * @brief  根据外设类型查找DMA映射表
 * @param  Peripheral: 外设类型
 * @retval DMA_Mapping_t*: 查找到的映射表指针,未找到返回NULL
 * */
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
 * @param  NewState: enable-启动，disable-停止
 * @retval Null
 * */
void DMA_EnablePeripheral(DMA_Config_t config, FunctionalState NewState)
{
    // 这里不用再次判定，使用该函数时，外设已经在DMA_InitChannel中判定过了
    switch (config.peripheral) {
        case DMA_PERIPH_ADC1:
            ADC_DMACmd(ADC1, NewState);
            break;
        case DMA_PERIPH_ADC2:
            ADC_DMACmd(ADC2, NewState);
            break;
        case DMA_PERIPH_USART1_TX:
            USART_DMACmd(USART1, (uint16_t)m->request_flag, NewState);
            break;

        case DMA_PERIPH_USART1_RX:
            USART_DMACmd(USART1, (uint16_t)m->request_flag, NewState);
            break;
        case DMA_PERIPH_USART2_TX:
            USART_DMACmd(USART2, (uint16_t)m->request_flag, NewState);
            break;
        case DMA_PERIPH_USART2_RX:
            USART_DMACmd(USART2, (uint16_t)m->request_flag, NewState);
            break;
        case DMA_PERIPH_SPI1_TX:
            SPI_I2S_DMACmd(SPI1, (uint16_t)m->request_flag, NewState);
            break;
        case DMA_PERIPH_SPI1_RX:
            SPI_I2S_DMACmd(SPI1, (uint16_t)m->request_flag, NewState);
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
void DMA_StartTransfer(DNA_config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    DMA_SetCurrDataCounter(m->channel, config.DMA_BufferSize);
    DMA_Cmd(m->channel, ENABLE);
}

/**
 * @brief  停止DMA服务,并清理标志位
 * @param  DMAy_Channelx: DMA通道
 * @retval Null
 * */
void DMA_StopTransfer(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    DMA_Cmd(m->channel, DISABLE);
    DMA_ClearFlag(m->flagStatus);
    // return ((DMA_GetFlagStatus(m->flagStatus) == RESET)); // Test 使用 TRUE 表示清除成功
}

/**
 * @brief  DMA 通道传输状态检测
 * @param  DMAy_Channelx: DMA通道
 * @retval True: 传输完成
 * @retval False: 传输失败
 * */
bool DMA_IsTransferComplete(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    return (DMA_GetFlagStatus(m->flagStatus) != RESET);
}

/**
* @brief  获取当前DMA传输剩余数据大小
* @param config: DMA初始化结构体  
* @retval uint32_t: 当前剩余数据大小
* */
uint32_t DMA_GetCurrDataSize(DMA_Config_t config)
{
    const DMA_Mapping_t *m = DMA_Mapping_Find(config.Peripheral);
    if (m == DMA_PERIPH_SPI1_RX || m == DMA_PERIPH_USART1_RX || m == DMA_PERIPH_USART2_RX)
        return DMA_GetCurrDataCounter(m->channel);
    return 0;
}
