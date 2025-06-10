#include "stm32f10x.h"

typedef struct {
    uint8_t buffer[2][256]; // Double buffer for reception
    volatile uint8_t activeBuffer;   // Index of the active buffer
    volatile uint16_t dataLength[2]; // Length of data in each buffer
} SerialPortRxBuffer;

typedef struct {
    uint8_t buffer[2][256]; // Double buffer for transmission
    volatile uint8_t activeBuffer;   // Index of the active buffer
    volatile uint16_t dataLength[2]; // Length of data in each buffer
} SerialPortTxBuffer;

SerialPortRxBuffer rxBuffer;
SerialPortTxBuffer txBuffer;

