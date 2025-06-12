#ifndef SeralPort_H
#define SeralPort_H

void SerialPort_InitGPIO(void);
void USART_DMA_Init(uint32_t baurdrate);
uint8_t USART_DMA_Send(uint8_t* data, uint16_t length);

#endif // SeralPort_H