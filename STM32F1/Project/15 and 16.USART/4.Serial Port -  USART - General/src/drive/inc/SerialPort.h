#ifndef SeralPort_H
#define SeralPort_H

void UsartDMA_Init(USARTDMAHandle *handel, uint32_t baudrate);
void UsartDMA_Send(USARTDMAHandle *handel, uint8_t *data, uint16_t len);
void UsartDMA_GPIO_Config(USART_TypeDef *USARTx);
void UsartDMA_RX_IRQHandler(USARTDMAHandle *handel);


#endif // SeralPort_H



