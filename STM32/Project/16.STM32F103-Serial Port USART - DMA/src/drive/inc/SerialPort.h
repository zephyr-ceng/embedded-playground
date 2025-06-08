#ifndef SeralPort_H
#define SeralPort_H

void USART1_DMA_Config(void);
void USART1_Config(void);
void DMA_Send(uint8_t* data, uint16_t len);
void DMA_Receive_Start(void);
#endif // SeralPort_H