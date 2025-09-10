#include "SerialPort.h"
// #include "./inc/SerialPort.h"


/******************************* Base初始化 *******************************/    

#define USART1_REC_LEN 254  
uint32_t USART1_RX_BUF[USART1_REC_LEN]; // 接收缓冲区

// GPIO 初始化
CPIO_config_t Tx = {
    .GPIOx = GPIOA,
    .GPIO_Pin = GPIO_Pin_9,      // TX
    .GPIO_Mode = GPIO_Mode_AF_PP, // 复用推挽输出
    .GPIO_Speed = GPIO_Speed_50MHz
};
GPIO_InitModule(Tx);
GPIO_Config_t Rx = {
    .GPIOx = GPIOA,
    .GPIO_Pin = GPIO_Pin_10,      // RX
    .GPIO_Mode = GPIO_Mode_IN_FLOATING, // 浮空输入
    .GPIO_Speed = GPIO_Speed_50MHz
};
GPIO_InitModule(Rx);


// USART 初始化
USART_config_t USART_Config = {
    .USARTx = USART1,
    .USART_BaudRate = 115200,
};

USART_InitModule(USART_Config);


// DMA 初始化
DMA_Config_t USART_DMA = {
    .Peripheral = DMA_Peripheral_USART1_RX,
    .DMA_MemoryBaseAddr = (uint32_t)USART1_RX_BUF,
    .DMA_BufferSize = USART1_REC_LEN
};


DMA_InitChannel_MtoP(USART_DMA);
DMA_Cmd(DMA1_Channel5, ENABLE); // 使能DMA通道