/****************
 *@description: Oled display example using IIC
 *@author: zephyr
 *@date: 2025-08-07 14:48:03
 *@version: V1.0.0
 ****************/

 #include "./Module/inc/MyDelay.h"
 #include "string.h"
 #include "./Module/inc/OLED.h"
 
 int main(void)
 {
     OLED_Init();                                                                  // Initialize OLED display
     OLED_Clear();                                                                 // Clear the display
     OLED_ShowChar(0, 0, 'A');                                                     // Display character 'A' at row 0, column 0
     OLED_ShowString(0, 0, "Hello, World!");                                       // Display a string at row 0, column 0
     OLED_ShowSNumber(1, 0, -12345, 5);                                            // Display signed number at row 1, column 0
     OLED_ShowFloat(2, 0, 3.14159, 5, 2);                                          // Display float number at row 2, column 0 with 2 decimal places
     OLED_ShowHexNumber(3, 0, 0xABCD, 4);                                          // Display hexadecimal number at row 3, column 0
     OLED_ShowStringScroll(4, 0, "Scrolling Text", strlen("Scrolling Text"), 200); // Scroll a string across the display
 }
 
 
 // TODO: OLED待测试
 