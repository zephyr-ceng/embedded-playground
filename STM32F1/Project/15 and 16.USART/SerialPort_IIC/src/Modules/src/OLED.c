/****************
 *@description: Oled Module Implementation
 *@author: zephyr
 *@date: 2025-08-07 14:49:21
 *@version: V1.0.0
 ****************/

#include "./Modules/inc/OLED.h"
#include "stm32f10x.h"
#include "./BSP_Drive/inc/I2C.h"
#include "./Modules/inc/OLED_Font.h"
#include "./BSP_Drive/inc/MyDelay.h"
#include "stdio.h"
#include "string.h"

#define OLED_ADDRESS 0x78              // OLED's I2C address
extern const uint8_t OLED_F8x16[][16]; // Font data

// OLED  write cmd using I2C1
void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t data[2];
    data[0] = 0x00; // Command mode
    data[1] = cmd;
    I2C_Send_Data(I2C1, OLED_ADDRESS, data, 2);
}

// OLED write data using I2C1
void OLED_WriteData(uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = 0x40; // Data mode
    buffer[1] = data;
    I2C_Send_Data(I2C1, OLED_ADDRESS, buffer, 2);
}

// OLED Initialization
void OLED_Init(void)
{
    Delay_ms(1000);        // Wait for OLED to power up
    I2C_Config_Init(I2C1); // Initialize I2C

    // Send initialization commands to OLED
    OLED_WriteCmd(0xAE); // Display off
    OLED_WriteCmd(0xD5); // Set display clock divide ratio/oscillator frequency
    OLED_WriteCmd(0x80); // Set divide ratio
    OLED_WriteCmd(0xA8); // Set multiplex ratio
    OLED_WriteCmd(0x3F); // 1/64 duty
    OLED_WriteCmd(0xD3); // Set display offset
    OLED_WriteCmd(0x00); // No offset
    OLED_WriteCmd(0x40); // Set display start line
    OLED_WriteCmd(0x8D); // Charge pump setting
    OLED_WriteCmd(0x14); // Enable charge pump
    OLED_WriteCmd(0x20); // Memory addressing mode
    OLED_WriteCmd(0x00); // Horizontal addressing mode
    OLED_WriteCmd(0xA1); // Segment remap
    OLED_WriteCmd(0xC8); // COM output scan direction
    OLED_WriteCmd(0xDA); // COM pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x81); // Contrast control
    OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xD9); // Pre-charge period
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB); // VCOMH deselect level
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0xA4); // Entire display ON (resume to RAM content display)
    OLED_WriteCmd(0xA6); // Normal display (not inverted)
    OLED_WriteCmd(0xAF); // Display ON
}

// clear OLED Display
void OLED_Clear(void)
{
    for (uint8_t page = 0; page < 8; page++) {
        OLED_WriteCmd(0xB0 + page); // Set page address
        OLED_WriteCmd(0x00);        // Set lower column address
        OLED_WriteCmd(0x10);        // Set higher column address

        for (uint8_t col = 0; col < 128; col++) {
            OLED_WriteData(0x00); // Clear data
        }
    }
}

// OLED Display set postion
void OLED_SetPosition(uint8_t x, uint8_t y)
{
    OLED_WriteCmd(0xB0 + y);                 // Set page address
    OLED_WriteCmd(0x00 + (x & 0x0F));        // Set lower column address
    OLED_WriteCmd(0x10 + ((x >> 4) & 0x0F)); // Set higher column address
}

/**
 * @brief  Display a character on OLED
 * @param  rows: Row number (0-7)
 * @param  cols: Column number (0-15)
 * @param  ch: Character to display (ASCII)
 * @retval Null
 * */
void OLED_ShowChar(uint8_t rows, uint8_t cols, char ch)
{
    if (ch < ' ' || ch > '~') return; // Check if character is in valid range

    uint8_t index = ch - ' '; // Calculate index in font array
    for (uint8_t i = 0; i < 16; i++) {
        OLED_SetPosition((cols - 1) * 8, (rows - 1));
        OLED_WriteData(OLED_F8x16[index][i]); // Write each byte of the character
        rows++;
        if (rows >= 8) { // Move to next page if needed
            rows = 0;
            cols += 1; // Move to next column
        }
    }
}

/**
 * @brief  Display a string on OLED
 * @param  rows: Starting row number (0-7)
 * @param  cols: Starting column number (0-15)
 * @param  str: Pointer to the string to display
 * @retval Null
 * */
void OLED_ShowString(uint8_t rows, uint8_t cols, const char *str)
{
    // while(*str)  '\0' 的ASCII码是0, 所以可以自动处理字符串
    while (*str) {
        OLED_ShowChar(rows, cols, *str++); // Display each character
        cols++;
        if (cols >= 16) { // Move to next row if needed
            cols = 0;
            rows++;
            if (rows >= 8) break; // Stop if we exceed the display height
        }
    }
}
/**
 * @brief  Display a string on OLED with specified length
 * @param  rows: Starting row number (0-7)
 * @param  cols: Starting column number (0-15)
 * @param  str: Pointer to the string to display
 * @param  len: Length of the string to display
 * @retval Null
 * */
void OLED_ShowStringN(uint8_t rows, uint8_t cols, const char *str, uint8_t len)
{
    for (uint8_t i = 0; i < len && str[i] != '\0'; i++) {
        OLED_ShowChar(rows, cols, str[i]); // Display each character
        cols++;
        if (cols >= 16) { // Move to next row if needed
            cols = 0;
            rows++;
            if (rows >= 8) break; // Stop if we exceed the display height
        }
    }
}

/**
 * @brief  Display a string on OLED with scrolling effect
 * @param  rows: Starting row number (0-7)
 * @param  cols: Starting column number (0-15)
 * @param  str: Pointer to the string to display
 * @param  len: Length of the string to display
 * @param  scrollTime: Time in milliseconds for each scroll step
 * @retval null
 * */
void OLED_ShowStringScroll(uint8_t rows, uint8_t cols, const char *str, uint8_t len, uint16_t scrollTime)
{
    // Scroll the string across the OLED display
    for (uint8_t offset = 0; offset < len; offset++) {
        for (uint8_t col = 0; col < 16; col++) {
            uint16_t index = (col + offset) % len;       // Calculate the index for scrolling
            OLED_ShowChar(rows, cols + col, str[index]); // Display character at the current
        }
        Delay_ms(scrollTime); // Delay for scrolling effect
    }
}

/**
 * @brief  Display number on OLED
 * @param  rows: Starting row number (0-7)
 * @param  cols: Starting column number (0-15)
 * @param  num: Number to display
 * @retval Null
 * */
void OLED_ShowSNumber(uint8_t rows, uint8_t cols, int32_t num, uint8_t width)
{
    if (width > 15) width = 15; // Limit width to maximum 16 characters
    char buffer[16];
    int len;
    if (num < 0) {
        len = snprintf(buffer, sizeof(buffer), "-%0*ld", width, -num); // Handle negative numbers
    } else {
        len = snprintf(buffer, sizeof(buffer), "%0*ld", width, num); // Handle positive numbers
    }
    if (len < 0 || len >= sizeof(buffer))  // Check for snprintf errors
    {
        strcpy(buffer, "Error"); // Fallback in case of error
    }
    OLED_ShowString(rows, cols, buffer);
}

/**
 * @brief  Display float number on OLED
 * @param  rows: Starting row number (0-7)
 * @param  cols: Starting column number (0-15)
 * @param  num: Float number to display
 * @param  precision: Number of decimal places to display
 * @retval Null
 * */
void OLED_ShowFloat(uint8_t rows, uint8_t cols, float num, uint8_t width, uint8_t precision)
{
    if (width + precision > 16) precision = 16 - width; // Limit width to maximum 16 characters
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%*.*f", width, precision, num); // Format float with specified precision
    OLED_ShowString(rows, cols, buffer);
}

/**
* @brief  Display hexadecimal number on OLED
* @param  rows: Starting row number (0-7)
* @param  cols: Starting column number (0-15)
n* @param  num: Hexadecimal number to display
* @param  width: Width of the hexadecimal number (number of digits)
* @retval Null
* */
void OLED_ShowHexNumber(uint8_t rows, uint8_t cols, uint32_t num, uint8_t width)
{
    if (width > 15) width = 15; // Limit width to maximum 8 characters for 32-bit hex
    char buffer[16];
    int len;
    len = snprintf(buffer, sizeof(buffer), "%0*X", width, (unsigned int)num); // Format as hexadecimal
    if (len < 0 || len >= sizeof(buffer)) // Check for snprintf errors
    {
        strcpy(buffer, "Error"); // Fallback in case of error
    }
    OLED_ShowString(rows, cols, buffer);
}