#ifndef SeralPort_H
#define SeralPort_H

void SerialPort_Init(uint32_t baudrate);
void SerialPort_SendData(char *data, uint16_t length);
uint32_t SerialPort_ReceiveData(char *data);

#endif // SeralPort_H