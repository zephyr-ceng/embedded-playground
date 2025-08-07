/****************
 *@description: Oled display example using IIC
 *@author: zephyr
 *@date: 2025-08-07 14:48:03
 *@version: V1.0.0
****************/


#include "./BSP_Drive/inc/MyDelay.h"
#include "string.h"
#include "./Modules/inc/OLED.h"

int main(void)
{
    OLED_Init(); // Initialize OLED display
    OLED_Clear(); // Clear the display
    OLED_ShowString(0, 0, "Hello, World!"); // Display a string at row 0, column 0
}
