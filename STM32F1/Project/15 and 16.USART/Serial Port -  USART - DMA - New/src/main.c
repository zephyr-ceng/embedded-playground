#include "stm32f10x.h"
#include "./drive/inc/SerialPort.h"
#include "./drive/inc/MyDelay.h"
#include "./drive/inc/OLED.h"
#include "string.h"


char msg[] = "Hello, STM32!"; 
char buffer[] = {0}; 

int main(void){
    OLED_Init();
    SerialPort_Init(115200); // Initialize Serial Port with 115200 baud rate
    OLED_ShowString(0, 0, "STM32F103");
    while (1)
    {
        SerialPort_SendData(msg, strlen(msg)); // Send data over Serial Port
        Delay_ms(1000); 

        SerialPort_ReceiveData(buffer); // Receive data into buffer
        OLED_ShowString(2,1, buffer); // Display received data on OLED
        memset(buffer, 0, sizeof(buffer)); // Clear buffer for next reception
        Delay_ms(1000); 
    }
    return 0;
}