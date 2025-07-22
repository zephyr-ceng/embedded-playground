#include "stm32f10x.h"
#include "./drive/inc/SerialPort.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "string.h"

#define USART_DMA_RX_BUFFER_SIZE 128
uint8_t rxBuffer[USART_DMA_RX_BUFFER_SIZE]; // 接收缓冲区
USARTDMAHandle usartHandle = {
    .USARTx = USART1,
    .DMAx_channel = DMA1_Channel5,
    .USART_IRQn = USART1_IRQn,
    .rxBuffer = rxBuffer,
    .bufSize = USART_DMA_RX_BUFFER_SIZE,
    .onReceive = onReceive // 初始化时不设置回调函数
};

void onReceive(uint8_t *data, uint16_t len) {
    // 处理接收到的数据
    OLED_ShowString(1, 1, (char *)data); // 在OLED上显示接收到的数据
    OLED_ShowString(2, 1, (char *)len); // 在OLED上显示接收到的数据
}

int main(void){
    OLED_Init();
    UsartDMA_Init(&usartHandle, 115200); // 初始化USART DMA
    UsartDMA_Send(USART1, (uint8_t *)"Hello, USART DMA!\r\n", 20); // 发送数据
    while (1)
    {

    return 0;
}


void USART1_IRQHandler(void)
{
    UsartDMA_RX_IRQHandler(&usartHandle); // 处理USART1中断
}   